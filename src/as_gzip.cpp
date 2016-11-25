/*
	2015.08.05 created by dancal
*/
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "../include/as_gzip.hpp"

namespace Gzip {

	int get_byte(z_stream &stream) {
  		if (stream.avail_in == 0) {
    		return EOF;
  		}
  		stream.avail_in--;
  		return *(stream.next_in)++;
	}

	char *gzdecode(const char *data, int &len) {
		z_stream stream;
  		stream.zalloc = (alloc_func) Z_NULL;
  		stream.zfree = (free_func) Z_NULL;

		long limit = 0;
		unsigned long plength=0, length;
  		int status;
  		unsigned int factor = 4, maxfactor = 16;
		char *s1 = NULL, *s2 = NULL;
		plength = limit;

		do {

	    	stream.next_in = (Bytef *)data;
    		stream.avail_in = (uInt)len + 1; /* there is room for \0 */

			length = plength ? plength : (unsigned long)len * (1 << factor++);
    		//length = (unsigned long)len * (1 << factor++);
    		s2 = (char *)realloc(s1, length);
	    	if (!s2) {
    	  		if (s1) free(s1);
      			return NULL;
		    }
			s1 = s2;

			stream.next_out = (Bytef*)s2;
			stream.avail_out = (uInt)length;

		    /* init with -MAX_WBITS disables the zlib internal headers */
    		status = inflateInit2(&stream, -MAX_WBITS);
    		if (status == Z_OK) {
      			status = inflate(&stream, Z_FINISH);
      			if (status != Z_STREAM_END) {
        			inflateEnd(&stream);
        			if (status == Z_OK) {
		          		status = Z_BUF_ERROR;
        			}
      			} else {
        			status = inflateEnd(&stream);
      			}
    		}
  		} while (status == Z_BUF_ERROR && (!plength) && factor < maxfactor);

	  	if (status == Z_OK) {
		    len = stream.total_out;
		    // shrink the buffer down to what we really need since this can be 16
		    // times greater than we actually need.
    		s2 = (char *)realloc(s2, len + 1);
    		//assert(s2);
    		s2[len] = '\0';
    		return s2;
  		}

  		free(s2);
  		return NULL;
	}

	std::string compress_gzip(const std::string& str, int compressionlevel ) {
	    z_stream zs;                        // z_stream is zlib's control structure
    	memset(&zs, 0, sizeof(zs));

	    if (deflateInit2(&zs, compressionlevel,Z_DEFLATED,MOD_GZIP_ZLIB_WINDOWSIZE + 16, MOD_GZIP_ZLIB_CFACTOR, Z_DEFAULT_STRATEGY) != Z_OK ) {
	        //throw(std::runtime_error("deflateInit2 failed while compressing."));
            return "";
    	}

	    zs.next_in = (Bytef*)str.data();
    	zs.avail_in = str.size();           // set the z_stream's input

	    int ret;
    	char outbuffer[32768];
	    std::string outstring;

    	// retrieve the compressed bytes blockwise
	    do {
    	    zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        	zs.avail_out = sizeof(outbuffer);

	        ret = deflate(&zs, Z_FINISH);

    	    if (outstring.size() < zs.total_out) {
        	    // append the block to the output string
            	outstring.append(outbuffer, zs.total_out - outstring.size());
        	}
	    } while (ret == Z_OK);

    	deflateEnd(&zs);

	    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
    	    //std::ostringstream oss;
        	//oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
	        //throw(std::runtime_error(oss.str()));
            return "";
    	}

	    return outstring;
	}

}
