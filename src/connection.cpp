//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright 2012 Red Hat, Inc.
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <stdio.h>
#include <sstream>
#include <vector>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/connection.hpp"
#include "../include/request_handler.hpp"

namespace http {
namespace server3 {

std::string FormatTime(std::string fmt, boost::posix_time::ptime now) {
	static std::locale loc(std::cout.getloc(),
		new boost::posix_time::time_facet(fmt.c_str()));

	std::stringstream ss;
	ss.imbue(loc);
	ss << now;
	return ss.str();
}

void base_connection::log_request() {
/*
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	std::string addrstr = peer.address().to_string();
	std::string timestr = FormatTime("%d/%b/%Y:%H:%M:%S", request_.tstamp);
	printf("%s - - [%s -0000] \"%s %s HTTP/%d.%d\" %d %lu\n",
      	     addrstr.c_str(),
	     timestr.c_str(),
	     request_.method.c_str(),
	     request_.uri.c_str(),
	     request_.http_version_major,
	     request_.http_version_minor,
	     reply_.status,
	     reply_.content.size());
*/

}

boost::asio::ip::tcp::socket& connection::socket() {
	return socket_;
}

/*
void connection::handle_timeout(error_code const & err){
  if(!err) { // timeout
    socket_.close(); // cause handle_read receive error_code 'Operation Canceled'
  }
}
*/
void connection::start() {
	read_more();
}

void connection::read_more() {
	socket_.async_read_some(boost::asio::buffer(buffer_),
	    strand_.wrap(
	      boost::bind(&connection::handle_read, shared_from_this(),
	        boost::asio::placeholders::error,
	        boost::asio::placeholders::bytes_transferred)));
}

void connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred) {
	if (e) {
	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The connection class's destructor closes the socket.
		return;
	}

	boost::tribool result;
	boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
			request_, buffer_.data(), buffer_.data() + bytes_transferred);

	if (result) {
		keepalive_ = request_.want_keepalive();
		request_.tstamp = boost::posix_time::second_clock::universal_time();

		request_handler_.handle_request(request_, reply_, keepalive_);
		boost::asio::async_write(socket_, reply_.to_buffers(),
			strand_.wrap(
				boost::bind(&connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error)));

		//log_request();

	} else if (!result) {
		reply_ = reply::stock_reply(reply::bad_request);
		boost::asio::async_write(socket_, reply_.to_buffers(),
			strand_.wrap(
				boost::bind(&connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error)));
	} else {
		read_more();
	}
	
	//std::cerr << "Handle Read - Connection shared_ptr count: " << shared_from_this().use_count() << std::endl;

}

void connection::handle_write(const boost::system::error_code& e) {
	if (e) {
// No new asynchronous operations are started. This means that all shared_ptr
// references to the connection object will disappear and the object will be
// destroyed automatically after this handler returns. The connection class's
// destructor closes the socket.
		return;
	}

	if (keepalive_) {
		reset();
		read_more();
	} else {
		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}

	//std::cerr << "Handle Read - Connection shared_ptr count: " << shared_from_this().use_count() << std::endl;
	//std::cout << "3. " << shared_from_this().get() << " has " << shared_from_this().use_count() << " references" << std::endl;

}


} // namespace server3
} // namespace http
