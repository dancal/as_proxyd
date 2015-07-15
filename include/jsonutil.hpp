#include <string>
#include <map>
#include <vector>
#include <jansson.h>

using namespace std;
using namespace boost;

namespace WP {
class JSON {

	public:
    static void JSON_FREE( json_t *root ) {
		if ( root ) {
			json_decref(root);
		}
	}

    static json_t *GET_JSON_OBJECT( const json_t *root, const std::string key ) {

        json_t *obj = json_object_get( root, key.c_str() );
	    return json_deep_copy(obj);

    };

    static json_t *GET_JSON_OBJECT_NO_COPY( const json_t *root, const std::string key ) {

        json_t *obj = json_object_get( root, key.c_str() );
        return obj;

    };
    static int GET_JSON_INT_VALUE( const json_t *root, const std::string json_key ) {

        json_t *obj = json_object_get( root, json_key.c_str() );
        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return fast_atou(json_string_value(obj));
            } else if ( type == JSON_INTEGER ) {
                return json_integer_value(obj);
            }

        }

        return 0;
    };

    static int GET_JSON_BOOLEAN_VALUE( const json_t *root, const std::string json_key, int def ) {

        json_t *obj = json_object_get( root, json_key.c_str() );
        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_TRUE ) {
                return 1;
            } else if ( type == JSON_FALSE ) {
                return 0;
            } else if ( type == JSON_INTEGER ) {
                return json_integer_value(obj);
            }

        }

        return def;
    };
    static int GET_JSON_INT_VALUE_DEFAULT( const json_t *root, const std::string json_key, int def ) {

        json_t *obj = json_object_get( root, json_key.c_str() );
        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return fast_atou(json_string_value(obj));
            } else if ( type == JSON_INTEGER ) {
                return json_integer_value(obj);
            }

        }

        return def;
    };

    static double GET_JSON_REAL_VALUE( const json_t *root, const std::string json_key, float def ) {

        json_t *obj = json_object_get( root, json_key.c_str() );
        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return fast_atoud(json_string_value(obj));
            } else if ( type == JSON_INTEGER ) {
                return json_integer_value(obj);
            } else if ( type == JSON_REAL ) {
                return json_real_value(obj);
            }

        }

        return def;
    };

    static std::string GET_JSON_STRING_VALUE( const json_t *root, const std::string json_key ) {

        json_t *obj = json_object_get( root, json_key.c_str() );
        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return json_string_value(obj);
            } else if ( type == JSON_INTEGER ) {
                return intToString(json_integer_value(obj));
            }

        }

        return "";
    };

    static std::string GET_JSON_STRING_VALUE_DEF( const json_t *root, const std::string json_key, std::string def ) {

        json_t *obj = json_object_get( root, json_key.c_str() );
        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return json_string_value(obj);
            } else if ( type == JSON_INTEGER ) {
                return intToString(json_integer_value(obj));
            }

        }

        return def;
    };

    static int GET_JSON_OBJ_INT_VALUE( const json_t *obj ) {

        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return fast_atou(json_string_value(obj));
            } else if ( type == JSON_INTEGER ) {
                return json_integer_value(obj);
            }

        }

        return 0;
    };

    static float GET_JSON_OBJ_REAL_VALUE( const json_t *obj ) {

        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return fast_atoud(json_string_value(obj));
            } else if ( type == JSON_INTEGER ) {
                return json_integer_value(obj);
            } else if ( type == JSON_REAL ) {
                return json_real_value(obj);
            }

        }

        return 0;
    };
    static std::string GET_JSON_OBJ_STRING_VALUE( const json_t *obj ) {

        if ( obj ) {
            int type    = json_typeof( obj );
            if ( type == JSON_STRING ) {
                return json_string_value(obj);
            } else if ( type == JSON_INTEGER ) {
                return intToString(json_integer_value(obj));
            }

        }

        return 0;
    };

	static std::string getFileText( std::string sFile ) {
    	ifstream inFile;
	    inFile.open( sFile.c_str() );//open the input file

    	stringstream strStream;
	    strStream << inFile.rdbuf();//read the file
		cout << strStream.str() << endl;
    	return strStream.str();//str holds the content of the file
	}

};
}
