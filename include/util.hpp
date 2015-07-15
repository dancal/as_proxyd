#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
 
#include <ctime>
#include <cstdlib>
#include <cstring>

#include <stdio.h>

#include <algorithm>
#include <map>
#include <boost/unordered_map.hpp>

using namespace std;
using std::string;

const static std::string baseKey = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

bool isHangul(const char *text);
bool url_decode(const std::string& in, std::string& out);
int string2int(std::string str);

void WriteLogFile(const char *format, ...);
std::string RemoveChars(const std::string& source, const std::string& chars);

void map_dump(std::map<std::string, std::string>& map);
std::string map_get(boost::unordered_map<std::string, std::string>& map, const std::string key, const std::string def);
std::vector<std::string> str_split(const string& str, const string& delimiter);
std::vector<std::string> split(std::string aString, std::string feildSep);
unsigned int fast_atou(const char *str);
double fast_atoud(const char *str);
std::string intToString( double nVal );
unsigned int nowtimeint();
unsigned int string2timeint( const std::string timestr );
std::string nowDateString();
std::string yesterdayYYMMDD();
int GetIndex(char * pString, char search);
int BaseToDec(char* number, int base);
int ConvertBase(string number,int currentBase, int targetBase);
std::time_t getFileTime( std::string filename );
std::vector<std::string> globVector(const string& pattern);
std::time_t getLastWriteTime( std::string sPath, std::string sFileName );
std::vector<std::string> vector_unique_list( std::vector<std::string>& input );
