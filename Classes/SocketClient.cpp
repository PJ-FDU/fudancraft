#pragma once
#include "SocketClient.h"
#include <iostream>

SocketClient::SocketClient():socket_(io_service_),
endpoint_(asio::ip::address_v4::from_string("127.0.0.1"), 10000)
{
	asio::error_code error;
	try 
	{
		socket_.connect(endpoint_,error);
		if (error)
			throw asio::system_error(error);
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in connection: " << e.what() << "\n";
	}
	start();
}

void SocketClient::start()
{
	socket_.async_read_some(asio::buffer(data_, max_length),
		std::bind(&SocketClient::handle_read, this,
			std::placeholders::_1,
			std::placeholders::_2)
	);
}

void SocketClient::send_data(std::string s)
{
	asio::error_code error;
	socket_.write_some(asio::buffer(s), error);
}

std::string SocketClient::read_data()
{
	auto tmp = message_;
	message_.clear();
	return tmp;
}

void SocketClient::handle_read(const asio::error_code& error, size_t bytes_transferred)
{
	if ((asio::error::eof == error) ||
		(asio::error::connection_reset == error))
	{
	}
	else
	{
		socket_.async_read_some(asio::buffer(data_, max_length),
		                        std::bind(&SocketClient::handle_read, this,
		                                  std::placeholders::_1,
		                                  std::placeholders::_2));
		message_ += std::string(data_);
		std::cout << message_ << std::endl;
	}
}
