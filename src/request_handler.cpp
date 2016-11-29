//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// by dancal

#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <cstdlib>
#include <stdint.h>

#include <jansson.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/thread/thread.hpp>  
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include <aerospike/as_error.h>

#include "../include/mime_types.hpp"
#include "../include/microtime.hpp"
#include "../include/reply.hpp"
#include "../include/request.hpp"
#include "../include/request_handler.hpp"

using namespace std;
using namespace pasl;
using namespace pasl::util;

//global
std::string as_proxyd_uptime;

namespace http {

	namespace server3 {

		using std::string;
		using boost::property_tree::ptree;

		extern std::string FormatTime(std::string fmt, boost::posix_time::ptime now);

		void request_handler::parse_query(const std::string& query, boost::unordered_map<std::string, std::string>& map){

		    if ( query.length() <= 0 || query.empty() ) {
        		return;
		    }

		    std::vector<std::string> vars;
		    boost::split(vars, query, boost::is_any_of("&"));

		    BOOST_FOREACH(string& pair, vars){
        		std::string::size_type sep = pair.find("=",0);

		        string key, value;

		        if (sep != std::string::npos) {
        		    key = pair.substr(0, sep);

		            value = pair.substr(sep+1);
        		    map[key] = value;
		        }
		    }

		}

		bool request_handler::url_decode(const std::string& in, std::string& out) {
		    out.clear();
		    out.reserve(in.size());
		    for (std::size_t i = 0; i < in.size(); ++i) {
			    if (in[i] == '%') {
					if (i + 3 <= in.size()) {
				  		int value = 0;
				        std::istringstream is(in.substr(i + 1, 2));
				        if (is >> std::hex >> value) {
				        	out += static_cast<char>(value);
          				i += 2;
        				} else {
          					return false;
        				}
      				} else {
        				return false;
      				}
	    		} else if (in[i] == '+') {
	      			out += ' ';
    			} else {
      				out += in[i];
    			}
	 	 	}
  			return true;
		}

		request_handler::request_handler( AS_CONTROL *pasOP ) {
			asOP			= pasOP;	
		}

		void request_handler::handle_request(const request& req, reply& rep, bool keepalive) {

			microtime::microtime_t __max_exec_start_time__  = microtime::now();

			std::string request_path;
			std::string urlfile;

			boost::unordered_map<std::string, std::string> params;

		  	// Decode url to path.
            if ( url_decode(req.uri, request_path) ) {

			    //Get Params
			    string::size_type pos = request_path.find_first_of("?");
			    if (pos != string::npos) {
    				urlfile = request_path.substr(0, pos);
					parse_query(request_path.substr(pos+1), params);
			    }

			}

			std::string AS_API			= urlfile;
			boost::algorithm::to_lower(AS_API);

			std::string AS_NAMESPACE	= map_get(params, "ns", "");
			std::string AS_SET			= map_get(params, "set", "");
			std::string AS_KEY			= map_get(params, "key", "");
			std::string AS_BIN_NAMES	= map_get(params, "bins", "");

			std::map<std::string,int> mBinNames;
            std::vector<std::string> vBinsNames;
            boost::split(vBinsNames, AS_BIN_NAMES, boost::is_any_of(","));
			BOOST_FOREACH( const std::string& pair, vBinsNames) {
				mBinNames[pair]			= 1;
			}


			json_t *root				= json_object();

			int nAsRet = AEROSPIKE_ERR;
		  	if ( AS_API == "/status" ) {

				json_object_set_new_nocheck(root, "proxy_code", json_integer(100) );
				json_object_set_new_nocheck(root, "proxy_status", json_string("OK") );

			} else if ( AS_API == "/get" ) {

				// ok
                // ok
                int ttl =0, gen = 0;

                json_t *bins 	= json_object();
                AS_OBJECT *as   = asOP->GetAsObject();
                if ( as ) {
                    nAsRet          = as->GET( AS_NAMESPACE, AS_SET, AS_KEY, bins, ttl, gen );
                    if ( json_object_size(bins) > 0 ) {
                        json_object_set_new_nocheck(root, "results", bins );
                    } else {
                        json_decref(bins);
                    }
                } else {
                    json_decref(bins);
                }

			}
			json_object_set_new_nocheck(root, "took", json_real( microtime::seconds_since(__max_exec_start_time__) ));

			//build http response
			rep.headers.clear();
			rep.status = reply::ok;

			char *result            = json_dumps(root, JSON_COMPACT | JSON_ENCODE_ANY | JSON_REAL_PRECISION(8) );
			rep.content.append( result, strlen(result) );
            free(result);

            rep.headers.push_back(header("Content-Length", boost::lexical_cast<std::string>(rep.content.size())));
            rep.headers.push_back(header("Content-Type", "application/json; charset=UTF-8"));

			if (keepalive)
				rep.headers.push_back(header("Connection", "Keep-Alive"));
			else
				rep.headers.push_back(header("Connection", "close"));

            json_decref(root);

		}

	}

}
