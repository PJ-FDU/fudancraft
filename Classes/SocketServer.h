#pragma once
#define ASIO_STANDALONE
#include "asio.hpp"
#include <thread>
#include <iostream>
#include <condition_variable>
#include <deque>
#include "socket_message.h"
#include "GameMessageWrap.h"
#include "cocos2d.h"
using asio::ip::tcp;

typedef std::shared_ptr<tcp::socket> socket_ptr;

class SocketServer;
class TcpConnection
	: public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::shared_ptr<TcpConnection> pointer;
//	~TcpConnection();
	static pointer create(asio::io_service& io_service, SocketServer* parent);
	tcp::socket& socket();

	void start();

	void write_data(std::string s);

	std::string read_data();


	void do_close();
private:

	void handle_read_header(const asio::error_code& error);

	void handle_read_body(const asio::error_code& error);


	TcpConnection(asio::io_service& io_service, SocketServer* parent);;


	void delete_from_parent();

	tcp::socket socket_;
	SocketServer* parent;
	
	socket_message read_msg_;
	std::deque<socket_message> read_msg_deque_;
	std::condition_variable data_cond_;
	std::mutex mut_;

};

class SocketServer
{
public:

	static SocketServer* create(int port = 8008);
//	~SocketServer() { acceptor_.close(); io_service_->stop(); }
	void close() {

		connections_.clear();
		io_service_->stop();
		acceptor_.close();
		delete io_service_;
		io_service_ = new asio::io_service;
		
	}
	std::vector<TcpConnection::pointer> get_connection() const;

	void remove_connection(TcpConnection::pointer p);
	void button_start();

	int connection_num()const { return connections_.size(); }
private:
	SocketServer(int port);
	void start_accept();

	void handle_accept(TcpConnection::pointer new_connection,
	                   const asio::error_code& error);

	void loop_process();


	tcp::acceptor acceptor_;
	std::vector<TcpConnection::pointer> connections_;
	int connection_num_;

	static asio::io_service* io_service_;

	std::thread *thread_, *button_thread_;

	std::condition_variable data_cond_;
};

