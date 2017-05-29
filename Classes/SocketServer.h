#pragma once
#define ASIO_STANDALONE
#include "asio.hpp"
#include <thread>
#include <iostream>
#include <condition_variable>
#include "socket_message.h"
#include "GameMessageWrap.h"
using asio::ip::tcp;

typedef std::shared_ptr<tcp::socket> socket_ptr;

class SocketServer;
class TcpConnection
	: public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::shared_ptr<TcpConnection> pointer;
	~TcpConnection();
	static pointer create(asio::io_service& io_service, SocketServer* parent)
	{		
		return pointer(new TcpConnection(io_service,parent));
	}
	tcp::socket& socket(){return socket_;}

	void start()
	{
		asio::async_read(socket_,
			asio::buffer(read_msg_.data(), socket_message::header_length),
			std::bind(&TcpConnection::handle_read_header, this,
				std::placeholders::_1));
	}
	void write_data(std::string s)
	{
		socket_message msg;
		msg.body_length(s.size());
		memcpy(msg.body(), &s[0u], msg.body_length());
		msg.encode_header();
		asio::write(socket_,
			asio::buffer(msg.data(), msg.length()));
	}
	std::string read_data()
	{
		while (!data_flag);
		auto ret = std::string(read_msg_.body(), read_msg_.body_length());
		data_flag = false;
		return ret;
	}


	void do_close()
	{
		socket_.close();
	}
private:

	void handle_read_header(const asio::error_code& error)
	{
		if (!error && read_msg_.decode_header())
		{
			std::cout << "here\n";
			asio::async_read(socket_,
				asio::buffer(read_msg_.body(), read_msg_.body_length()),
				std::bind(&TcpConnection::handle_read_body, this,
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
			std::cout << "read:";
			std::cout.write(read_msg_.body(), read_msg_.body_length());
			std::cout << "\n";
			while (data_flag);
			asio::async_read(socket_,
				asio::buffer(read_msg_.data(), socket_message::header_length),
				std::bind(&TcpConnection::handle_read_header, this,
					std::placeholders::_1));
		}
		else
		{
			do_close();
		}
	}



	TcpConnection(asio::io_service& io_service, SocketServer* parent) : 
		socket_(io_service), parent(parent),
		data_flag{ false } {
		std::cout << "new tcp" << std::endl;
		
	};

//	void handle_connection(int n);
//	void handle_read(const asio::error_code& error,
//	                 size_t bytes_transferred);


	void delete_from_parent();

	tcp::socket socket_;
	SocketServer* parent;

	static int player_num;
	
	std::string message_;
	enum { max_length = 1024 };
	char data_[max_length]{ 0 };
	
	socket_message read_msg_;
	bool data_flag;

};

class SocketServer
{
public:

	static SocketServer* create(int port=8008)
	{
//		asio::io_service io_service;
		auto s= new SocketServer(port);
		s->thread_ = new std::thread(
			std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run),
				io_service_));
		return s;
	}

	std::vector<TcpConnection::pointer> get_connection() const;

	void remove_connection(TcpConnection::pointer p);
	void button_start()
	{
		for (auto i = 0; i < connections_.size(); i++)
			connections_[i]->write_data("PLAYER" + std::to_string(i));
		connection_num = connections_.size();
		this->button_thread_ = new std::thread(std::bind(&SocketServer::loop_process,this));
	}

private:	
//	void run(){thread_ = std::thread(std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run), &acceptor_.geis));
//	}
//	SocketServer(asio::io_service& io_service_);
	SocketServer(int port) :
		acceptor_(*io_service_, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}
	void start_accept();

	void handle_accept(TcpConnection::pointer new_connection,
	                   const asio::error_code& error);

	void loop_process()
	{
		while (true)
		{	
			if (connections_.size() != connection_num)
				throw std::exception{ "lost connection" };
			std::vector<std::string> ret;
			/*for (auto r : connections_)
			{
				GameMessageSet game_message_set;
				auto ret = r->read_data();
				game_message_set.ParseFromString(ret);
				std::cout << game_message_set.DebugString() << std::endl;
				r->write_data(ret);
			}*/
			for (auto r : connections_)
				ret.push_back(r->read_data());
			std::cout << "read all message\n";
			auto game_msg = GameMessageWrap::combine_message(ret);
			/*GameMessageSet game_message_set;
			game_message_set.ParseFromString(game_msg);
			std::cout << game_message_set.DebugString() << std::endl;*/
			for (auto r : connections_)
				r->write_data(game_msg);
		}
	}


	tcp::acceptor acceptor_;
	std::vector<TcpConnection::pointer> connections_;
	int connection_num;
	
	static asio::io_service* io_service_;
	std::thread* thread_, *button_thread_;
	std::condition_variable data_cond_;
};

//class Server{
//public:
//	static Server* create() {return new Server;  };
//	SocketServer* socket_server()const { return socket_server_; };
//	std::vector<TcpConnection::pointer> get_connection() const
//	{
//		return socket_server_->get_connection();
//	}
//private:
//	Server()
//	{
//		socket_server_ = new SocketServer(io_service);		
//		thread_= new std::thread(std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run), &io_service));
//	}
//
//	SocketServer* socket_server_;
//	asio::io_service io_service;
//	std::thread* thread_;
//};