//
// server.cpp
// ~~~~~~~~~~
//
// Copyright 2012 Red Hat, Inc.
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <stdio.h>
#include <cstdlib>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <mecab.h>

#include <boost/filesystem/operations.hpp>
#include <ctime>
#include <iostream>

#include "../include/server.hpp"

extern std::string as_proxyd_uptime;

namespace http {
namespace server3 {

// Every time the timer fires we will generate a new frame and send it to all subscribers.
void server::handle_timer() {

	if ( !isHandleStop ) {

		asOP_->CheckAsServersStatus();

		// Wait for next timeout.
	    timer_.expires_from_now(boost::posix_time::milliseconds(AS_HEALTH_TIMER));
		timer_.async_wait(boost::bind(&server::handle_timer, this));
	}
}

server::server(const std::string& address, unsigned int port, AS_CONTROL *pasOP, std::size_t thread_pool_size)
  : asOP_(pasOP),
	thread_pool_size_(thread_pool_size),
    signals_(io_service_),
    acceptor_(io_service_),
    new_connection_(),
    request_handler_(pasOP), 
    timer_(io_service_) {

	// Register to handle the signals that indicate when the server should exit.
  	// It is safe to register for the same signal multiple times in a program,
  	// provided all registration for the specified signal is made through Asio.
  	signals_.add(SIGINT);
  	signals_.add(SIGTERM);

#if defined(SIGQUIT)
  	signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  	signals_.async_wait(boost::bind(&server::handle_stop, this));

  	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  	char portstr[10];
  	snprintf(portstr, sizeof(portstr), "%u", port);
  	boost::asio::ip::tcp::resolver resolver(io_service_);
  	boost::asio::ip::tcp::resolver::query query(address, portstr);
  	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

  	acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::linger(false, 0));
    acceptor_.set_option(boost::asio::ip::tcp::no_delay(true));
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::keep_alive(true));
  	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  	acceptor_.bind(endpoint);
  	acceptor_.listen();

	std::time_t t;
	std::time(&t);
    const std::tm* timeinfo = std::localtime ( &t ) ;
    char yyyymmdd[28] ;
    std::strftime( yyyymmdd, sizeof(yyyymmdd), "%Y-%m-%d %H:%M:%S", timeinfo );
    as_proxyd_uptime	= std::string( yyyymmdd );

  	// Start the timer used to generate outgoing frames.
  	//timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.expires_from_now(boost::posix_time::milliseconds(AS_HEALTH_TIMER));
  	timer_.async_wait(boost::bind(&server::handle_timer, this));

  	start_accept();

	isHandleStop	= false;
}

void server::run() {

  	// Create a pool of threads to run all of the io_services.
  	std::vector<boost::shared_ptr<boost::thread> > threads;
  	for (std::size_t i = 0; i < thread_pool_size_; ++i) {
    	boost::shared_ptr<boost::thread> thread(new boost::thread( boost::bind(&boost::asio::io_service::run, &io_service_)) );
    	threads.push_back(thread);
		asOP_->mthreadList[thread->get_id()] = i;
  	}

	asOP_->Initialize();

    WriteLogFile("%-40s : %s", "Started..........", "OK");
	fflush(stdout);

  	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i) {
		threads[i]->join();
	}

}

void server::start_accept() {
	new_connection_.reset(new connection(io_service_, request_handler_));
	acceptor_.async_accept(new_connection_->socket(), new_connection_->peer, boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e) {

	if (!e) {
		new_connection_->start();
	}

	start_accept();

}

void server::handle_stop() {
	isHandleStop	= true;
	io_service_.stop();
}

} // namespace server3
} // namespace http
