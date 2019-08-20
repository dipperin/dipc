//
// Created by zhou.yang on 2018/10/10.
//
#include "StringUtil.h"
#include <sys/time.h>

namespace dipc {

    std::string randomString(int size) {
        const static std::string alph = "0123456789abcdefghigklmnopqrstuvwxyz";
        std::string str;
        str.resize(size);

        struct timeval tv;  
        gettimeofday(&tv,NULL);
        srand(tv.tv_usec);
        for (size_t i = 0; i < size; ++i) {
            str[i] = alph[rand() % size];
        }
        return str;
    }

    int split( const std::string & srcStr, std::vector<std::string> & destArray, const std::string & delimiter ){
        if( srcStr.empty() ){
            return 0;
        }
        std::string::size_type startPos = srcStr.find_first_not_of( delimiter );
        size_t lengthOfDelimiter = delimiter.length();
        while( std::string::npos != startPos ){
            std::string::size_type nextPos = srcStr.find( delimiter, startPos );
            std::string str;
            if( std::string::npos != nextPos ){
                str = srcStr.substr( startPos, nextPos - startPos );
                nextPos += lengthOfDelimiter;
            }
            else{
                str = srcStr.substr( startPos );
            }
            startPos = nextPos;
            if( !str.empty() ){
                destArray.push_back( str );
            }
        }
        return destArray.size();
    }
}
