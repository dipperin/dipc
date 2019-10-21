
#include <dipc/dipc.hpp>
#include <gtest/gtest.h>
#include <boost/algorithm/string.hpp>
#include <algorithm>
using namespace dipc;


#include <string>
std::string TestAddr(const char* addr){
    // Address adr = Address(addr, 22);
    // hexStringSameWithVM(addr);
    // return adr.toString();
    return "";
}

std::string TestCharToAddrToStr(const char* addr){
    //return CharToAddress2Str(addr);
    return "";
}

TEST(GTEST,RESULT){
    std::string in = "0x00005586B883Ec6dd4f8c26063E18eb4Bd228e59c3E9";
    //boost::to_lower(in);
    //std::transform(in.begin(), in.end(), in.begin(), std::tolower );
    //ASSERT_EQ(in , TestCharToAddrToStr("0x00005586B883Ec6dd4f8c26063E18eb4Bd228e59c3E9"));
    ASSERT_EQ(in, TestAddr("0x00005586B883Ec6dd4f8c26063E18eb4Bd228e59c3E9"));
}