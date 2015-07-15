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
#include <aerospike/as_arraylist.h>
#include <aerospike/as_list.h>
#include <aerospike/as_boolean.h>

#include <stdio.h>
#include <jansson.h>
#include <iostream>
#include <string>
#include <stdint.h>
#include <errno.h>

#include <vector>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include <jemalloc/jemalloc.h>


#include "INIReader.hpp"
#include "config.hpp"
#include "util.hpp"

#include "jsonutil.hpp"

using namespace std;
using namespace boost;

class AS_OBJECT {
	
	public:
		AS_OBJECT( std::vector<std::string> &as_server_list, int as_port, uint32_t timeout_ms ) {
			
			isalive_ 	= false;
		    as_			= NULL;

		    srand(unsigned(time(NULL)));
		    std::random_shuffle( as_server_list.begin(), as_server_list.end() );

			as_config_init(&as_config_);
    		BOOST_FOREACH(string& server, as_server_list) {
		        as_config_add_host(&as_config_, server.c_str(), as_port);
		    }

			as_config_.policies.read.timeout = timeout_ms;
			as_config_.policies.apply.timeout = timeout_ms;
    		as_config_.lua.cache_enabled = false;
		    //strcpy(config.lua.system_path, "modules/lua-core/src");
		    //strcpy(config.lua.user_path, "src/test/lua");
		    as_policies_init(&as_config_.policies);

		    as_error_reset(&as_err_);
    		aerospike_init(as_, &as_config_);

		    as_                      = aerospike_new(&as_config_);
			//as_log_set_level(AS_LOG_LEVEL_INFO);

		    if (aerospike_connect(as_, &as_err_) == AEROSPIKE_OK) {
				isalive_				= true;
			}
		}
		~AS_OBJECT() {
		    // free
		    as_error_reset(&as_err_);
		    if ( aerospike_close(as_, &as_err_) == AEROSPIKE_OK ) {
        		aerospike_destroy(as_);
		    } else {
        		aerospike_destroy(as_);
		    }

		}
		bool AS_OBJECT_STATUS() {

			// AEROSPIKE_ERR_PARAM
			// AEROSPIKE_ERR_CLIENT
			// AEROSPIKE_ERR
			if ( !isalive_ ) {
				return false;
			}
            as_status status;

            as_record r, *rec = &r;
            as_record_init(&r, 0);

            as_key key;
            as_key_init(&key, "ns", "set", "key");
			try {
        	    status  = aerospike_key_get(as_, &as_err_, NULL, &key, &rec);
			} catch (...) {
				status	= AEROSPIKE_ERR;	
			}
            as_key_destroy(&key);

			if ( status == AEROSPIKE_ERR_PARAM ) {
				return false;
			} else if ( status == AEROSPIKE_ERR_CLIENT ) {
				return false;
			} else if ( status == AEROSPIKE_ERR ) {
				return false;
			}			
			return true;
		}

		void AS_OBJECT_RECONNECT() {
            as_error_reset(&as_err_);
		    if (aerospike_connect(as_, &as_err_) == AEROSPIKE_OK) {
				isalive_		= true;
			} else {
				isalive_		= false;
			}
		}

		const char * doubleToLPSTR(double x){
			const int size = 20;
			char *cs = new char[size];
			string s;
			stringstream ss;
			ss << x;
			s = ss.str();
			const char * tempAr = s.c_str();
			for (int i = 0; i < size; i++){
			    cs[i] = tempAr[i];
			}

			return cs;
		}

		int as_string_len(as_string *string) {
		    if (string->value == NULL) {
        		return 0;
		    }

			return string->len;
		}

		char *as_string_to_byte( as_string * string ) {

			int nLength	= as_string_len( string );
			char *cstr = new char[nLength+1];
			memset(cstr, 0, nLength+1);
			memcpy(cstr, string->value, nLength);

			return cstr;
		}

		void data_convert_to_json_v1( as_record *rec, json_t *bins ) {

			// counter for the bins
			as_record_iterator it;
			as_record_iterator_init(&it, rec);

			try {
				while (as_record_iterator_has_next(&it)) {

					// we expect the bins to contain a list of [action,timestamp]
					as_bin *bin 	= as_record_iterator_next(&it);				
					char *name 		= as_bin_get_name(bin);
					as_val *value 	= (as_val *) as_bin_get_value(bin);

					int nValueType  = as_val_type(value);
                    if ( nValueType == AS_BOOLEAN ) {
						json_object_set_new_nocheck(bins, name, json_boolean( as_boolean_toval(as_boolean_fromval(value))) );
					} else if ( nValueType == AS_INTEGER ) {
						json_object_set_new_nocheck(bins, name, json_integer( as_integer_toint(as_integer_fromval(value))) );
					} else if ( nValueType == AS_STRING ) {
						json_object_set_new_nocheck(bins, name, json_string( as_string_tostring(as_string_fromval(value))) );
					} else if ( nValueType == AS_BYTES ) {
						json_error_t error;
						char *buffer 	= reinterpret_cast<char*>(as_bytes_get(as_bytes_fromval(value)));
						json_object_set_new_nocheck(bins, name, json_loads(buffer, 0, &error) );
					} else if ( nValueType == AS_LIST ) {
						json_object_set_new_nocheck(bins, name, json_string("") );
					} else if ( nValueType == AS_MAP ) {
						json_object_set_new_nocheck(bins, name, json_string("") );
					} else {
						json_object_set_new_nocheck(bins, name, json_string("") );
					}
			
				}

            } catch (...) {
            }

			as_record_iterator_destroy(&it);
			
		}

		int GET( std::string ns, std::string set, std::string userid, json_t *bins, int& ttl, int& gen ) {

			if ( !isalive_ ) {
				return -1;
			}

			as_error_reset(&as_err_);

			as_status status;

			as_record r, *rec = &r;
			as_record_init(&r, 0);

            as_key key;
			as_key_init(&key, ns.c_str(), set.c_str(), userid.c_str());

			status	= aerospike_key_get(as_, &as_err_, NULL, &key, &rec);

			as_key_destroy(&key);

			//printf("%s @ %s[%s:%d]", err.message, err.func, err.file, err.line);
			if ( status != AEROSPIKE_OK ) {
				return status;
            }

			data_convert_to_json_v1( rec, bins );

			gen	= rec->gen;
			ttl	= rec->ttl;

			as_record_destroy(rec);

			return status;

		}

		int PUT( std::string ns, std::string set, std::string userid, json_t *json_load, int ttl ) {

			if ( !isalive_ ) {
				return -1;
			}
			size_t nCount	= json_object_size(json_load);
			if ( nCount <= 0 ) {
				return 0;
			}

			as_error_reset(&as_err_);

			as_status status;
			status  = AEROSPIKE_ERR_QUERY;

            as_key key;
			as_key_init(&key, ns.c_str(), set.c_str(), userid.c_str());

			as_record rec;
			as_record_inita(&rec, nCount);

			if ( ttl ) {
				rec.ttl = ttl;
			}

			int recCount = 0;
            const char *name;
            json_t *value;
			try {
	            json_object_foreach(json_load, name, value) {

					uint8_t* buffer;
					size_t	 buffer_size;
    	            switch (json_typeof(value)) {
    	            	case JSON_INTEGER:
							recCount++;
							as_record_set_integer(&rec, name, as_integer_new(json_integer_value(value))); 
							break;
						case JSON_STRING:
							recCount++;
							as_record_set_str(&rec, name, json_string_value(value) );
							break;
						case JSON_REAL:
							recCount++;
							as_record_set_str(&rec, name, doubleToLPSTR(json_real_value(value)));
							break;
						case JSON_NULL:
							recCount++;
							as_record_set_nil(&rec, name);
							break;
						case JSON_TRUE:
							recCount++;
							as_record_set_integer(&rec, name, as_integer_new(1)); 
							break;
						case JSON_FALSE:
							recCount++;
							as_record_set_integer(&rec, name, as_integer_new(0)); 
							break;
						case JSON_OBJECT:
						case JSON_ARRAY:
							recCount++;
							buffer		= reinterpret_cast<uint8_t*>(json_dumps(value, 0));
							buffer_size	= strlen((char*)buffer);
							as_record_set_bytes(&rec, name, as_bytes_new_wrap(buffer, buffer_size + 1, true) );
							break;
						default:
							break;
            		}
	
				}

				if ( recCount ) {
					status	= aerospike_key_put(as_, &as_err_, NULL, &key, &rec);
				}

			} catch (...) {
			}

			as_key_destroy(&key);
			as_record_destroy(&rec);

			return status;
		}

		int REMOVE( std::string ns, std::string set, std::string userid ) {

			if ( !isalive_ ) {
				return -1;
			}

			as_error_reset(&as_err_);

			as_status status;

            as_key key;
			as_key_init(&key, ns.c_str(), set.c_str(), userid.c_str());

			try {
				status = aerospike_key_remove(as_, &as_err_, NULL, &key);
			} catch (...) {
			}

			as_key_destroy(&key);

			return status;
		}

		char *get_as_status_msg(int nRet) {
			return as_error_string((as_status)nRet);
		}

		bool isAliveAsServer() {
			if ( as_err_.code == AEROSPIKE_ERR_TIMEOUT ) {
				//isalive_ = false;
			} else if ( as_err_.code == AEROSPIKE_ERR_CLIENT ) {
				isalive_ = false;
			} else if ( as_err_.code == AEROSPIKE_ERR_CLUSTER_CHANGE ) {
				isalive_ = false;
			} else if ( as_err_.code == AEROSPIKE_ERR_CLUSTER ) {
				isalive_ = false;
			} else if ( as_err_.code == AEROSPIKE_ERR_INVALID_HOST ) {
				isalive_ = false;
			}
			return isalive_;
		}

	protected:
    	as_error as_err_;
	    aerospike *as_;
    	as_config as_config_;

		bool isalive_;
};
