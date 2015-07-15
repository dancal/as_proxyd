//
// main.cpp
// ~~~~~~~~
//
// Copyright 2012 Red Hat, Inc.
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <locale.h>
#include <iostream>
#include <string>
#include <locale>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <algorithm>
#include <stdio.h> 
#include <stdlib.h>
#include <regex.h>

#include <libgen.h> 
#include <stdlib.h> 

#include "../include/server.hpp"
#include "../include/request_handler.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;

int main() {

	char linkname[512];
	char buf[512];
	pid_t pid;
	int ret;

	pid = getpid();
	snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid);
	ret = readlink(linkname, buf, 512);
	buf[ret] = 0;

	std::string ROOT_PATH			= buf;
	const size_t last_slash_idx = ROOT_PATH.find_last_of("\\/");
	ROOT_PATH.erase(last_slash_idx, ROOT_PATH.length() );
	ROOT_PATH.append("/../");

	//config file
	std::string the_config_file	= ROOT_PATH;
	the_config_file.append(CONFIG_FILE);
	if ( !boost::filesystem::exists(the_config_file) ) {
		WriteLogFile("[ERROR] check config file path %s", the_config_file.c_str() );
		return 1;
	}

	try {

		WriteLogFile("==================================================");
	    WriteLogFile("%-40s : %s", "wp_bid_engine version", SERVER_VER );

    	INIReader reader( the_config_file );
        if (reader.ParseError() < 0) {
			WriteLogFile("%-40s : %s", "Can't load", the_config_file.c_str() );
	        return 1;
    	} else {
			WriteLogFile("%-40s : %s", "load config file", the_config_file.c_str() );
		}

		std::string health_file	= reader.Get("server", "status", "");
    	std::string host    	= reader.Get("server", "host", "0.0.0.0");
        int port         		= reader.GetInteger("server", "port", 4000);
		int max_thread			= reader.GetInteger("server", "max_thread", 8);

		std::string as_servers	= reader.Get("aerospike", "servers", "127.0.0.1");
		int as_port				= reader.GetInteger("aerospike", "port", 3000);
		int as_timeout			= reader.GetInteger("aerospike", "timeout", 500);

		boost::replace_all( as_servers, "\"", "");
		boost::replace_all( as_servers, "\t", "");
		boost::replace_all( as_servers, "\n", "");
		boost::replace_all( as_servers, "\r", "");
		boost::replace_all( as_servers, " ", "");

		AS_CONTROL *asOP		= new AS_CONTROL(as_servers, as_port, as_timeout, max_thread, health_file, &reader);

		// Initialise the server.
		http::server3::server s(host, port, asOP, max_thread);
		WriteLogFile("%-40s : %s", "Starting..........", "OK");

		s.run();

		delete asOP;

		WriteLogFile("%-40s : %s\n", "Stop..........", "OK");

	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
