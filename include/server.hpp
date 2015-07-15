#ifndef HTTP_SERVER3_SERVER_HPP
#define HTTP_SERVER3_SERVER_HPP

#include <string>
#include <vector>
#include <iostream>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "connection.hpp"
#include "request_handler.hpp"

using namespace std; 
using std::string;

namespace http {
namespace server3 {

/// The top-level class of the HTTP server.
class server : private boost::noncopyable {

public:
	explicit server(const std::string& address, unsigned int port, AS_CONTROL *asOP, std::size_t thread_pool_size);
	void run();
	bool isHandleStop;

private:

	void handle_timer();

	void start_accept();
	void handle_accept(const boost::system::error_code& e);
	void handle_stop();

	AS_CONTROL *asOP_;

	/// The number of threads that will call io_service::run().
	std::size_t thread_pool_size_;

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service_;

	/// The signal_set is used to register for process termination notifications.
	boost::asio::signal_set signals_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor acceptor_;

	/// The next connection to be accepted.
	connection_ptr new_connection_;

	/// The handler for all incoming requests.
	request_handler request_handler_;

	boost::asio::deadline_timer timer_;
};

} // namespace server3
} // namespace http

#endif
