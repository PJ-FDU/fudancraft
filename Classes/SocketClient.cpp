#pragma once
#include "SocketClient.h"
#include <iostream>






void SocketClient::send_data(std::string s)
{
	asio::error_code error;
	socket_.write_some(asio::buffer(s), error);
}



