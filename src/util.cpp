#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
 
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <fstream>      // std::ifstream
#include <algorithm>

#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/tokenizer.hpp>  

#include <string>
#include <locale>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <stdarg.h>
#include <glob.h>

#include "../include/config.hpp"
#include "../include/util.hpp"

using namespace std;
using std::string;
using std::vector;

char symbols[37] = "0123456789ABCDEFGHIJKLMNoPQRSTUVWXYZ";
const int MAX_BASE = 36; //Highest base allowed (make sure there are enough symbols first!)

bool isHangul(const char *text) {
    bool hangul = true;
    
    for (unsigned int i = 0; i < strlen(text); i++) {
        if ((text[i] & 0x80) != 0x80) {
            hangul = false;
        }
    }
    return hangul;
}

bool url_decode(const std::string& in, std::string& out) {
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

int string2int(std::string s) {
	int i;
	stringstream ss(s);
	if( (ss >> i).fail() ) {
		return 0;
	}
	return i;
}

unsigned int nowtimeint() {
	time_t t = time(NULL);
    return mktime( localtime(&t) );

//    time_t t = time(NULL);
//    return t + localtime( &t )->tm_gmtoff;

}

std::string yesterdayYYMMDD() {

	char yestDt[11];
    time_t now = time(NULL);
	now = now - (24*60*60);

	struct tm *t = localtime(&now);
	sprintf(yestDt, "%04d%02d%02d", t->tm_year + 1900, t->tm_mon+1, t->tm_mday);
	
    return yestDt;
}

unsigned int string2timeint( const std::string timestr ) {
	struct tm time;
    strptime(timestr.c_str(), "%Y-%m-%d %H:%M:%S", &time);
	return timegm(&time);
}

std::string nowDateString() {
    char serverDateStr[11];
    time_t t = time(NULL);
	strftime(serverDateStr, 11, "%Y-%m-%d", localtime(&t));

	return serverDateStr;
}

void WriteLogFile(const char *format, ...) {
 
	#define MAX_PER_LOG_TATALL      512
    char szStr[MAX_PER_LOG_TATALL] = {0};

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);

  	va_list args;
  	va_start (args, format);
  	vsnprintf (szStr, MAX_PER_LOG_TATALL - 1, format, args);
  	va_end (args);
    // output
    szStr[MAX_PER_LOG_TATALL - 1] = '\0';

  	FILE* pFile = fopen( CONFIG_LOG_FILE, "a");
  	fprintf(pFile, "%s\t%s\n", buffer, szStr);
  	fclose(pFile);
 
  	printf("%s\t%s\n", buffer, szStr);

}

void map_dump(std::map<std::string, std::string>& map){
	std::string key, value;
    BOOST_FOREACH(boost::tie(key, value), map) {
    	printf("===== key : %s, value = %s\n", key.c_str(), value.c_str() );
	}
}	

std::string map_get(boost::unordered_map<std::string, std::string>& map, const std::string key, const std::string def) {
    boost::unordered_map<std::string, std::string>::iterator it = map.find(key);
	if (it != map.end()){
		return it->second;
	} else {
        return def;
	}
}

std::string RemoveChars(const std::string& source, const std::string& chars) {
	std::string result="";
	for (unsigned int i=0; i<source.length(); i++) {
		bool foundany=false;
		for (unsigned int j=0; j<chars.length() && !foundany; j++) {
        	foundany=(source[i]==chars[j]);
		}
		if (!foundany) {
        	result+=source[i];
		}
	}
	return result;
}

std::vector<std::string> str_split(const string& str, const string& delimiter) {
    std::vector<std::string> tokens;

    string::size_type lastPos = 0;
    string::size_type pos = str.find(delimiter, lastPos);

    while (string::npos != pos) {
        // Found a token, add it to the vector.
        //cout << str.substr(lastPos, pos - lastPos) << endl;
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = pos + delimiter.size();
        pos = str.find(delimiter, lastPos);
    }   

    tokens.push_back(str.substr(lastPos, str.size() - lastPos));
    return tokens;
}

unsigned int fast_atou(const char *str) {
	unsigned int val = 0;
	if ( str ) {
    	while(*str) {
        	val = (val << 1) + (val << 3) + *(str++) - 48;
	    }
	}
    return val;
}

double fast_atoud(const char *str) {
	try {
		if ( str ) {
			return strtod(str, NULL);
		} else {
			return 0;
		}
	} catch (...) {
		return 0;
	}
}

std::string intToString( double nVal ) {
    std::ostringstream ostr;
    ostr << nVal;
    return ostr.str();
}

//Finds the index of a particular character (if it exists) in an array. Returns last character on failure
//Used by BaseToDec to find face values
int GetIndex(char * pString, char search) {
        int index = 0;
        while(*pString != (char)0) {
            if(*pString==search)
                break;
            pString++;
            index++;
        }
        return index;
}

int BaseToDec(char* number, int base) {
        if(base<2||base>MAX_BASE)
            return 0; //Failed

        int NumLength = strlen(number);
        int PlaceValue, total = 0;
        //Work out the place value of the first digit (base^length-1)
        PlaceValue = (int)pow(base,NumLength-1);

        //For each digit, multiply by its place value and add to total
        for(int i=0;i<NumLength;i++) {
            total += GetIndex(symbols,*number)*PlaceValue;
            number++;
            PlaceValue /= base; //Next digit's place value (previous/base)
        }
        return total;
}

int ConvertBase(string number,int currentBase, int targetBase) {
    string temp = "";
    string answer;
    char curr;
    int value;
    long int total=0;
    for( int i=number.length()-1 ; i>= 0 ; i--) {
        curr = number[i];
        if( isdigit(curr) )
        {
            value = curr - '0';
        }
        else if( isalpha(curr) )
        {
            curr = tolower(curr);
            value = curr - 'a' + 10;
        }
        else {
            return 0;
        }
        total += value * pow(currentBase,number.length()-i-1);
    }
 
	//  cout << total << endl;
    // we now have the decimal value stored in total. time to convert it to the other base.
    //int power = 0;
    while(total)
    {
        value = total % targetBase;
        total /= targetBase;
        if( value < 10 )
        {
            temp += value + '0';
        }
        else
        {
            temp += value - 10 + 'a';
        }
    }
    for( int i=1 ; i<=temp.length() ; i++) {
        answer += temp[temp.length() - i];
    }
    return string2int(answer);

}

std::time_t getFileTime( std::string filename ) {

    boost::filesystem::path p( filename.c_str() ) ;
    if ( boost::filesystem::exists( p ) ) {
    	return boost::filesystem::last_write_time( p );
    }

	return 0;
};

std::vector<std::string> globVector(const string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    std::vector<string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

std::time_t getLastWriteTime( std::string sPath, std::string sFileName ) {
	
	boost::filesystem::path path( sPath + "/" + sFileName ) ;
	return boost::filesystem::last_write_time( path );

}

std::vector<std::string> vector_unique_list( std::vector<std::string>& input ) {

    std::vector<std::string> result;
    result.reserve(input.size());
    for (std::vector<std::string>::iterator itor = input.begin(); itor != input.end(); ++itor) {
        if (std::find(result.begin(), result.end(), *itor) == result.end()) {
            result.push_back(*itor);
        }
    }

    return result;

}
