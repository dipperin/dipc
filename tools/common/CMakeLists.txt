add_compile_options(-std=c++14)
include_directories(../include)
add_library(common STATIC Log.cpp StringUtil.cpp)