cmake_minimum_required(VERSION 3.5)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/AddrestTest.cpp ${CMAKE_BINARY_DIR}/AddrestTest.cpp)

message("current test dir :" ${CMAKE_CURRENT_SOURCE_DIR} "    current binary dir : " ${CMAKE_CURRENT_BINARY_DIR})
message("project_source_dir : " ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    # ${CMAKE_CURRENT_BINARY_DIR}/googletest-build/include
    # ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
    #${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/dipclib/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/dipclib/rt
    ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/boost/include
    # /usr/local/Cellar/cryptopp/8.1.0/include/cryptopp/

    # ${BINARY_DIR}/include/
)
# LINK_LIBRARIES(
#     cryptopp
# )

AUX_SOURCE_DIRECTORY(. DIR_SRC_TESTS)
add_executable(gtestwork ${DIR_SRC_TESTS})
TARGET_LINK_LIBRARIES(gtestwork gtest gtest_main)
#add_tests(gtestwork)
add_custom_command(TARGET gtestwork POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)
add_custom_command(TARGET gtestwork POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtestwork> ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:gtestwork>)

