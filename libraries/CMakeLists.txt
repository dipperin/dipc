message("cmake run libraries.cmake~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")

cmake_minimum_required(VERSION 3.5)

add_subdirectory(rt)

message("the CMAKE_C_FLAGS is: ${CMAKE_C_FLAGS}")
message("the CMAKE_CXX_FLAGS is: ${CMAKE_CXX_FLAGS}")

set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Oz -Wall --target=wasm32")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -Oz -Wall --target=wasm32")

message("lib dir ${CMAKE_CURRENT_SOURCE_DIR}")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dipclib/include/ DESTINATION ${BINARY_DIR}/include/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/boost/include/ DESTINATION ${BINARY_DIR}/include/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include/ DESTINATION ${BINARY_DIR}/include/)

add_subdirectory(libc)
add_subdirectory(libc++)
add_subdirectory(native)
