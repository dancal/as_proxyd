//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright 2012 Red Hat, Inc.
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_REQUEST_HANDLER_HPP
#define HTTP_SERVER3_REQUEST_HANDLER_HPP

#include <string>

#include <vector>

#include <map>
#include <iostream>
#include <sstream>
#include <cstdlib>

#include <boost/noncopyable.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "as_control.hpp"

using namespace std;
using std::string;

namespace http {
	namespace server3 {

		struct reply;
		struct request;
	
		/// The common handler for all incoming requests.
		class request_handler : private boost::noncopyable {
			public:
				/// Construct with a directory containing files to be served.
				explicit request_handler( AS_CONTROL *pasOP );

				bool url_decode(const std::string& in, std::string& out);
				void parse_query(const std::string& query, boost::unordered_map<std::string, std::string>& map);
				void handle_request(const request& req, reply& rep, bool keepalive);
			private:
				AS_CONTROL *asOP;

		};

	} // namespace server3
} // namespace http

#endif
