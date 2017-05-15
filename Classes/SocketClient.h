#pragma once
#define ASIO_STANDALONE
#include "asio.hpp"
#include <functional>
#include <thread>
using asio::ip::tcp;


class SocketClient
{
public:
	static SocketClient* create()
	{
		auto s = new SocketClient();
		s->thread_ = new std::thread(
			std::bind(static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run),
				&s->io_service_));
		return s;
	}
	void start();
	void send_data(std::string s);
	std::string read_data();

private:
	SocketClient();
	void handle_read(const asio::error_code& error,
	                 size_t bytes_transferred);

	asio::io_service io_service_;
	tcp::socket socket_;
	tcp::endpoint endpoint_;

	enum{max_length = 1024};

	std::string message_;
	char data_[max_length]{0};
	std::thread* thread_;
};
