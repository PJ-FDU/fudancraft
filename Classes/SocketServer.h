#pragma once
#define ASIO_STANDALONE
#include "asio.hpp"
#include <thread>
#include <iostream>
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

	void start();
	bool write_data(std::string s);
	std::string read_data();;

private:
	TcpConnection(asio::io_service& io_service, SocketServer* parent) : 
		socket_(io_service), parent(parent) {
		std::cout << "new tcp" << std::endl;
	};

	void handle_connection(int n);
	void handle_read(const asio::error_code& error,
	                 size_t bytes_transferred);


	void delete_from_parent();

	tcp::socket socket_;
	SocketServer* parent;

	static int player_num;
	
	std::string message_;
	enum { max_length = 1024 };
	char data_[max_length]{ 0 };

};

class SocketServer
{
public:

	static SocketServer* create()
	{
//		asio::io_service io_service;
		auto s= new SocketServer();
		s->thread_ = new std::thread(
			std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run),
				io_service_));
		return s;
	}

	std::vector<TcpConnection::pointer> get_connection() const;

	void remove_connection(TcpConnection::pointer p);
	void button_start();




private:	
//	void run(){thread_ = std::thread(std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run), &acceptor_.geis));
//	}
	SocketServer(asio::io_service& io_service_);
	SocketServer() :
		acceptor_(*io_service_, tcp::endpoint(tcp::v4(), 8008))
	{
		start_accept();
	}
	void start_accept();

	void handle_accept(TcpConnection::pointer new_connection,
	                   const asio::error_code& error);

	tcp::acceptor acceptor_;
	std::vector<TcpConnection::pointer> connections_;
	
	static asio::io_service* io_service_;
	std::thread* thread_;
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