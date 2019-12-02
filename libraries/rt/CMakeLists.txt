cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(LLVM_CONFIG_PATH ${BINARY_DIR}/dipc-llvm/tools/llvm-config)

set(C_DEFINES, "-D__wasm__ -DQUAD_PRECISION")

# set( CMAKE_C_FLAGS " -Wall ${CMAKE_C_FLAGS} ${C_DEFINES} -allow-sse  -msse -msse2 -msse4" )
# set( CMAKE_CXX_FLAGS " -Wall ${CMAKE_CXX_FLAGS} ${C_DEFINES} -allow-sse -msse -msse2 -msse4")
set( CMAKE_C_FLAGS " -Wall ${CMAKE_C_FLAGS} ${C_DEFINES} -allow-sse  " )
set( CMAKE_CXX_FLAGS " -Wall ${CMAKE_CXX_FLAGS} ${C_DEFINES} -allow-sse ")




# set( CMAKE_CXX_FLAGS " -Wall   -O3  -O3 -Wall   -allow-sse -std=c++14")
# set( CMAKE_C_FLAGS " -Wall   -O3  -O3 -Wall   -allow-sse")

#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CMAKE_CURRENT_SOURCE_DIR}/../libc++/libcxx/include")
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/include")

#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CMAKE_CURRENT_SOURCE_DIR}/../dipclib/include")

#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CMAKE_CURRENT_SOURCE_DIR}/../boost/include ")  #-nostdinc



set ( builtins_sources
   compiler_builtins.cpp
   fixtfti.c
   fixunstfti.c
   fixsfti.c
   fixdfti.c
   fixunssfti.c
   fixunsdfti.c
   floattidf.c
   floatuntidf.c
)


file ( GLOB builtins_headers "${CMAKE_CURRENT_SOURCE_DIR}*.h" )
list( APPEND builtins_sources ${builtins_headers} )

add_library ( rt STATIC ${builtins_sources} )

target_include_directories( rt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
                                      "${CMAKE_CURRENT_SOURCE_DIR}/../native/softfloat/source/include" )

message("the BINARY_DIR is: ${BINARY_DIR}")
message("the CMAKE_COMMAND is: ${CMAKE_COMMAND}")
add_custom_command( TARGET rt POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rt> ${BINARY_DIR}/lib )
#add_custom_command( TARGET native_rt POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:native_rt> ${BASE_BINARY_DIR}/lib )
#add_custom_command( TARGET export C_INCLUDE_PATH="${BEFORT_C_INCLUDE_PATH}" CPLUS_INCLUDE_PATH="{BEFORT_CPLUS_INCLUDE_PATH}" )
#set(ENV{C_INCLUDE_PATH} "${BEFORT_C_INCLUDE_PATH}")
#set(ENV{CPLUS_INCLUDE_PATH} "${BEFORT_CPLUS_INCLUDE_PATH}")
message("C_INCLUDE_PATH after ${C_INCLUDE_PATH}")
message("CPLUS_INCLUDE_PATH  after ${CPLUS_INCLUDE_PATH}")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${BINARY_DIR}/include FILES_MATCHING PATTERN "compiler_builtins.hpp")
