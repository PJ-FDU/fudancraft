#pragma once
#define ASIO_STANDALONE
#include "asio.hpp"
#include <functional>
#include <thread>

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
		return s;
	}
	void start()
	{
		start_connect();
	};

	std::vector<GameMessage> get_game_messages()
	{
		auto game_message_set_stirng = read_data();
		return GameMessageWrap::set_string_to_vector(game_message_set_stirng);
	}

	void send_game_message(const std::vector<GameMessage> &vec_game_msg)
	{
		auto set_string = GameMessageWrap::vector_to_set_stirng(vec_game_msg);
		write_data(set_string);
	}

	[[deprecated("just for test")]]
	void send_string(std::string s)
	{
		write_data(s);
	}

	[[deprecated("just for test")]]
	std::string get_string()
	{
		return read_data();
	};

	void do_close()
	{
		socket_.close();
	}

	int camp()const { return camp_; }
private:
	void write_data(std::string s)
	{
		socket_message msg;
		msg.body_length(s.size());
		memcpy(msg.body(), &s[0u], msg.body_length());
		msg.encode_header();
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
				char data[20] = {0};
				asio::error_code error;
				size_t length = socket_.read_some(asio::buffer(data, 20), error);
				if (error || length < 10)
					throw asio::system_error(error);
				camp_ = atoi(data + 10);
				std::cout << "camp: " << camp_;

				asio::async_read(socket_,
				                 asio::buffer(read_msg_.data(), socket_message::header_length),
				                 std::bind(&SocketClient::handle_read_header, this,
				                           std::placeholders::_1));
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
			data_flag = true;
			std::cout << "read completed\n";
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
		while (!data_flag);
		auto ret = std::string(read_msg_.body(), read_msg_.body_length());
		data_flag = false;
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
	bool data_flag;
//	asio::streambuf input_buffer_;
//	std::string message_;
//	std::vector<char> data_;
	std::thread* thread_;
	int camp_;
};
