#pragma once
#define ASIO_STANDALONE
#include "asio.hpp"
#include <functional>
#include <thread>
#include <chrono>
#include "cocos2d.h"

#include <iostream>
#include "socket_message.h"
#include "GameMessage.pb.h"
#include "GameMessageWrap.h"

using asio::ip::tcp;


class SocketClient
{
public:
	static SocketClient* create(std::string ip="127.0.0.1",int port = 8008)
	{
		auto s = new SocketClient(ip,port);
		s->thread_ = new std::thread(
			std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run),
				&s->io_service_));
		s->thread_->detach();
		return s;
	}
	void start()
	{
		start_connect();
	};
	[[deprecated("just for test")]]
	std::vector<GameMessage> get_game_messages()
	{
		auto game_message_set_stirng = read_data();
		return GameMessageWrap::set_string_to_vector(game_message_set_stirng);
	}

	[[deprecated("just for test")]]
	void send_game_message(const std::vector<GameMessage> &vec_game_msg)
	{
		auto set_string = GameMessageWrap::vector_to_set_stirng(vec_game_msg);
		write_data(set_string);
	}

	
	void send_string(std::string s)
	{
		write_data(s);
	}

	std::string get_string()
	{
		return read_data();
	};

	void do_close()
	{
		socket_.close();
	}


	int camp()const { while (!start_flag_); return camp_; }
	int total()const { while (!start_flag_); return total_; }

private:
	void write_data(std::string s)
	{
		socket_message msg;
		if (s.size() == 0)
		{
			s = std::string("\0");
			msg.body_length(1);
		}
		else
			msg.body_length(s.size());
		memcpy(msg.body(), &s[0u], msg.body_length());
		msg.encode_header();
		cocos2d::log("client send data: %s",msg.data());
		asio::write(socket_,
			asio::buffer(msg.data(), msg.length()));
	}


	void start_connect()
	{
		socket_.async_connect(endpoint_,
			std::bind(&SocketClient::handle_connect, this,
				std::placeholders::_1));

	}


	void handle_connect(const asio::error_code& error)
	{
		try
		{
			if (!error)
			{
				std::cout << "connected\n";
				char data[30] = {0};
				asio::error_code error;
				size_t length = socket_.read_some(asio::buffer(data, 30), error);
				if (error || length < 10)
					throw asio::system_error(error);
				char header[4 + 1] = "";
				strncat(header, data+10, 4);
				total_ = atoi(header);
				camp_ = atoi(data + 14);				
				start_flag_ = true;
				cocos2d::log("camp:%d, total:%d", camp_,total_);
				asio::async_read(socket_,
					asio::buffer(read_msg_.data(), socket_message::header_length),
					std::bind(&SocketClient::handle_read_header, this,
						std::placeholders::_1));
//				read_thread_ = new std::thread(std::bind(&SocketClient::start_read, this));
//				read_thread_->detach();
			}
			else
			{
				std::cerr << "failed to connect" << std::endl;
				throw asio::system_error(error);
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception in connection: " << e.what() << "\n";

		}
	}

	void start_read()
	{


		while(1)
		{
			std::lock_guard<std::mutex> lk{ mut };
			asio::error_code error;
			asio::read(socket_, asio::buffer(read_msg_.data(), socket_message::header_length),error);
			asio::read(socket_,	asio::buffer(read_msg_.body(), read_msg_.body_length()),error);
			data_flag = true;
			data_cond_.notify_one();
			std::cout << "read completed\n";
//			cocos2d::log("client receive completed: %s", read_msg_.data());
			while (data_flag);
		}


//		
//		
//		
//		
	}

	void handle_read_header(const asio::error_code& error)
	{
		if (!error && read_msg_.decode_header())
		{
			
			asio::async_read(socket_,
				asio::buffer(read_msg_.body(), read_msg_.body_length()),
				std::bind(&SocketClient::handle_read_body, this,
					std::placeholders::_1));
		}
		else
		{
			do_close();
		}
	}

	void handle_read_body(const asio::error_code& error)
	{
		if (!error)
		{
			std::lock_guard<std::mutex> lk{ mut };
			data_flag = true;
			data_cond_.notify_one();
			std::cout << "read completed\n";
			cocos2d::log("client receive completed: %s",read_msg_.data());
//			std::cout << "read:";
//			std::cout.write(read_msg_.body(), read_msg_.body_length());
//			std::cout << "\n";
			while (data_flag);
			asio::async_read(socket_,
				asio::buffer(read_msg_.data(), socket_message::header_length),
				std::bind(&SocketClient::handle_read_header, this,
					std::placeholders::_1));
		}
		else
		{
			do_close();
		}
	}

	std::string read_data()
	{
//		std::unique_lock<std::mutex> lk{ mut };
		while (!data_flag);
//			data_cond_.wait(lk); ;
		auto ret = std::string(read_msg_.body(), read_msg_.body_length());
		data_flag = false;
//		lk.unlock();
		return ret;
	}
private:
	SocketClient::SocketClient(std::string ip,int port) : socket_(io_service_),
	                               endpoint_(asio::ip::address_v4::from_string(ip), port),
	                               data_flag(false)
	{
		//	asio::error_code error;
		//	try 
		//	{
		//		socket_.connect(endpoint_,error);
		//		if (error)
		//			throw asio::system_error(error);
		//	}
		//	catch (std::exception& e)
		//	{
		//		std::cerr << "Exception in connection: " << e.what() << "\n";
		//	}
		start();
	}

	asio::io_service io_service_;
	tcp::socket socket_;
	tcp::endpoint endpoint_;
	socket_message read_msg_;

	
//	enum{max_length = 4};

	bool data_flag,start_flag_{false};

//	asio::streambuf input_buffer_;
//	std::string message_;
//	std::vector<char> data_;
	std::thread* thread_,*read_thread_;
	int camp_,total_;

	std::condition_variable data_cond_;
	std::mutex mut;
};
