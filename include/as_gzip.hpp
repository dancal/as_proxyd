/*
	2015.08.05 created by dancal
*/
#ifndef __GZIP_H__
#define __GZIP_H__
 
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define GZIP_HEADER_LENGTH 			10
#define GZIP_FOOTER_LENGTH 			8

#define MOD_GZIP_ZLIB_WINDOWSIZE 	15
#define MOD_GZIP_ZLIB_CFACTOR    	9
#define MOD_GZIP_ZLIB_BSIZE      	8096

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

static const int gz_magic[2] = {0x1f, 0x8b};

namespace Gzip {
	int get_byte(z_stream &stream);
	char *gzdecode(const char *data, int &len);
	std::string compress_gzip(const std::string& str, int compressionlevel = Z_BEST_COMPRESSION);
};
 
#endif // __GZIP_H__
