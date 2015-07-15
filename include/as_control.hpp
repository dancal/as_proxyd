//
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <fstream>

#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include <string>
#include <algorithm>
#include <iterator>

#include <locale>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_index.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_udf.h>
#include <aerospike/as_bin.h>
#include <aerospike/as_bytes.h>
#include <aerospike/as_error.h>
#include <aerospike/as_config.h>
#include <aerospike/as_key.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_password.h>
#include <aerospike/as_record.h>
#include <aerospike/as_record_iterator.h>
#include <aerospike/as_status.h>
#include <aerospike/as_string.h>
#include <aerospike/as_val.h>

#include "as_object.hpp"

using namespace std;
using std::string;

class AS_CONTROL {
	public:
		int port_;
		int timeout_;
		int threadCount_;
		bool as_health_;
		INIReader *reader_;

		std::string servers_;
		std::vector<std::string> as_server_list_;

		std::string health_file_;
		boost::unordered_map<boost::thread::id, int> mthreadList;
		boost::unordered_map<boost::thread::id, AS_OBJECT *> rAsList;

		AS_CONTROL( std::string servers, int port, int timeout, int threadCount, std::string health_file, INIReader *reader ) {
			servers_		= servers;
			health_file_	= health_file;
			port_			= port;
			timeout_		= timeout;
			threadCount_	= threadCount;
			as_health_		= false;
			reader_			= reader;
            boost::split(as_server_list_, servers, boost::is_any_of(","));
	
		};
		~AS_CONTROL() {
            boost::thread::id key;
            AS_OBJECT *value;
            BOOST_FOREACH(boost::tie(key, value), rAsList) {
                delete rAsList[key];
            }

		};

		AS_OBJECT *GetAsObject() {

			boost::thread::id nThreadID = boost::this_thread::get_id();
			if ( rAsList.count(nThreadID) <= 0 ) {
				rAsList[nThreadID]		= new AS_OBJECT( as_server_list_, port_, timeout_ );
			}

			return rAsList[nThreadID];

		}

		void Initialize() {
            boost::thread::id key;
	        int nVal;
    	    BOOST_FOREACH(boost::tie(key, nVal), mthreadList ) {
				if ( rAsList.count(key) <= 0 ) {
					rAsList[key]		= new AS_OBJECT( as_server_list_, port_, timeout_ );
				}
			}

		}

		bool GetAsHealthStatus() {
			if ( rAsList.size() <= 0 ) {
				return false;
			}

			if ( !healthFileExist() ) {
				return false;
			}

			CheckAsServersStatus();

            boost::thread::id key;
            AS_OBJECT *value;
            BOOST_FOREACH(boost::tie(key, value), rAsList) {
				
				AS_OBJECT *as	= value;
				if ( !as ) { 
					return false;
				}
				if ( !as->isAliveAsServer() ) {
					return false;
				}
            }
			return true;
		}

		void CheckAsServersStatus() {

            boost::thread::id key;
            AS_OBJECT *value;
            BOOST_FOREACH(boost::tie(key, value), rAsList) {
				if ( value && !value->AS_OBJECT_STATUS() ) {
					value->AS_OBJECT_RECONNECT();
				}
            }

		}

		std::string getTimeString( time_t stime ) {
			const std::tm* timeinfo = std::localtime ( &stime ) ;
			char yyyymmdd[28] ;
			std::strftime( yyyymmdd, sizeof(yyyymmdd), "%Y-%m-%d %H:%M:%S", timeinfo );
			std::string buffer = std::string( yyyymmdd );
			return buffer;
		}

		inline bool healthFileExist() {
		    boost::filesystem::path p( health_file_.c_str() ) ;
		    if ( boost::filesystem::exists( p ) ) {
				return true;
			} else {
				return false;
			}
		};
	
        std::string getConfigString(std::string section, std::string &name, std::string def ) {
            return reader_->Get( section, name, def );
        }
        int getConfigInt(std::string section, std::string name, int def ) {
            return reader_->GetInteger( section, name, def );
        }
        long getConfigFloat(std::string section, std::string &name, long def ) {
            return reader_->GetInteger( section, name, def );
        }
        bool getConfigBool(std::string section, std::string &name, int def ) {
            return reader_->GetBoolean( section, name, def );
        }

};
