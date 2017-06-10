#pragma once
#define ASIO_STANDALONE
#define ASIO_HAS_STD_CHRONO
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
	bool error()const { return error_flag_; }


	void do_close();
private:

	void handle_read_header(const asio::error_code& error);

	void handle_read_body(const asio::error_code& error);


	TcpConnection(asio::io_service& io_service, SocketServer* parent);;

	void check_timer();
	void delete_from_parent();

	tcp::socket socket_;
	SocketServer* parent;
	bool error_flag_{ false };	
	
	socket_message read_msg_;
	std::deque<socket_message> read_msg_deque_;
	std::condition_variable data_cond_;
	std::mutex mut_;
//	asio::steady_timer steady_timer_;

};

class SocketServer
{
public:

	/**
	 * \brief create a server on port number
	 * \param port port number, default 8008
	 * \return socket server
	 */
	static SocketServer* create(int port = 8008);
//	~SocketServer() { acceptor_.close(); io_service_->stop(); }
	/**
	 * \brief close the server
	 */
	void close();
	/**
	 * \brief 
	 * \return TcpConnection vector
	 */
	std::vector<TcpConnection::pointer> get_connection() const;

	/**
	 * \brief remove a connction, if there is a connction
	 * \param p tcp connection
	 */
	void remove_connection(TcpConnection::pointer p);
	/**
	 * \brief start the game
	 */
	void button_start();

	/**
	 * \brief 
	 * \return if error occured
	 */
	bool error() const;

	/**
	 * \brief 
	 * \return total connction number
	 */
	int connection_num() const;
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

	std::thread *thread_, *button_thread_{nullptr};
	std::mutex delete_mutex_;
	bool error_flag_{ false };
	std::condition_variable data_cond_;
};

