#include "SocketServer.h"


int TcpConnection::player_num = 0;
asio::io_service* SocketServer::io_service_ =new asio::io_service;

TcpConnection::~TcpConnection()
{
	std::cout << "delete";
	delete_from_parent();	
}

void TcpConnection::start()
{
	
//	std::thread t(std::bind(&TcpConnection::handle_connection, shared_from_this(), ++player_num));
//	t.detach();
	socket_.async_read_some(asio::buffer(data_, max_length),
		std::bind(&TcpConnection::handle_read, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2)
	);

}

void TcpConnection::handle_connection(int n)
{
//	init_connection(n);
	try
	{
		for (;;)
		{
			char data[max_length]{0};

			asio::error_code error;
			size_t length = socket_.read_some(asio::buffer(data), error);
			if (error == asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw asio::system_error(error); // Some other error.
			std::cout << "Player: " << n << std::endl;
			std::cout << data << std::endl;
			asio::write(socket_, asio::buffer(data, length));
//			std::cout << "done!\n";
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
		delete_from_parent();
	}
}

void TcpConnection::handle_read(const asio::error_code& error, size_t bytes_transferred)
{
	if ((asio::error::eof == error) ||
		(asio::error::connection_reset == error))
	{
		delete_from_parent();
	}
	else
	{
		socket_.async_read_some(asio::buffer(data_, max_length),
		                        std::bind(&TcpConnection::handle_read, shared_from_this(),
		                                  std::placeholders::_1,
		                                  std::placeholders::_2));
		message_ += std::string(data_);
	}
}


void TcpConnection::delete_from_parent()
{
	if(parent)
		shared_from_this()->parent->remove_connection(shared_from_this());
	parent = nullptr;
	
}

SocketServer::SocketServer(asio::io_service& io_service):
	acceptor_(io_service, tcp::endpoint(tcp::v4(), 8008))
{
	start_accept();
}

std::vector<TcpConnection::pointer> SocketServer::get_connection() const
{
	return connections_;
}

void SocketServer::remove_connection(TcpConnection::pointer p)
{
	//		connections_.erase(std::remove(connections_.begin(), connections_.end(), p), connections_.end());
	auto position = std::find(connections_.begin(), connections_.end(), p);

	if (position == connections_.end())
		std::cout << "delete not succ\n";
		
	else
		connections_.erase(position);
	std::cout << "delete succ\n";
}

void SocketServer::button_start()
{
	for (auto i = 0; i < connections_.size(); i++)
		connections_[i]->write_data("PLAYER" + std::to_string(i));
}

void SocketServer::start_accept()
{
	TcpConnection::pointer new_connection =
		TcpConnection::create(acceptor_.get_io_service(), this);

	acceptor_.async_accept(new_connection->socket(),
	                       std::bind(&SocketServer::handle_accept, this, new_connection,
	                                 std::placeholders::_1));
	//		std::cout << "start accept " << std::endl;
}

void SocketServer::handle_accept(TcpConnection::pointer new_connection, const asio::error_code& error)
{
	std::cout << "handle_accept\n";
	if (!error)
	{
		connections_.push_back(new_connection);
		std::cout << new_connection->socket().remote_endpoint().address()
			<< ":" << new_connection->socket().remote_endpoint().port() << std::endl;
		new_connection->start();
	}
	start_accept();
	//		std::cout << "handle accept\n";
}

bool TcpConnection::write_data(std::string s)
{
	try
	{
		asio::write(socket_, asio::buffer(s, s.length()));
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in write: " << e.what() << "\n";
	}
	return true;
}

std::string TcpConnection::read_data()
{
	auto ret = message_;
	message_ = "";
	return ret;
}




