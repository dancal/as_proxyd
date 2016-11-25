//
//  wp_base64.hpp

#ifndef _WP__BASE64_
#define _WP__BASE64_

#include <stdio.h>
#include <string>

using namespace std;

static const std::string kBase64String =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

class Base64 {
	public:
    	static std::string base64Encode(unsigned char const* , unsigned int length);
	    static std::string base64Decode(std::string const& string);
    	static size_t base64DecodeToData(const std::string& sourceString, void* destinationPointer, size_t destinationSize);
};

#endif
