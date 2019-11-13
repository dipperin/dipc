cmake_minimum_required(VERSION 3.5)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(WARNING "CMAKE_INSTALL_PREFIX is set to default path of ${CMAKE_INSTALL_PREFIX}, resetting to ${CMAKE_INSTALL_PREFIX}/dipc.cdt")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/dipc.cdt")
 elseif ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
   message(WARNING "CMAKE_INSTALL_PREFIX is explicitly set to /usr/local. This is not recommanded.")
endif()

include(GNUInstallDirs)

set(BIN_SUFFIX)
if (WIN32)
  set(BIN_SUFFIX .exe)
endif()


include(CMakeModules/LLVMExternalProject.cmake)
include(CMakeModules/BoostExternalProject.cmake)
include(CMakeModules/ToolsExternalProject.cmake)

message("the CMAKE_BINARY_DIR is:${CMAKE_BINARY_DIR}")

#configure_file(${CMAKE_SOURCE_DIR}/tools/Log.cpp ${CMAKE_BINARY_DIR}/tools/Log.cpp @ONLY)

# Download and unpack googletest at configure time
# configure_file(${CMAKE_SOURCE_DIR}/CmakeModules/CMakeLists-googletest.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
# message("CMAKE_GENERATOR  ${CMAKE_GENERATOR}   CMAKE_CURRENT_BINARY_DIR   ${CMAKE_CURRENT_BINARY_DIR} ")
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#     RESULT_VARIABLE  result
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

# if(result)
#    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
# endif()

# execute_process(COMMAND ${CAME_COMMAND} --build . 
#    RESULT_VARIABLE  result
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download  )

# execute_process(COMMAND ${CMAKE_COMMAND} --build .
#     RESULT_VARIABLE result
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

# if(result)
#    message(FATAL_ERROR "Build step for googletest failed: ${result}")
# endif()

# set(google_force_shared_crt ON CACHE BOOL "" FORCE)

# add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
#                  ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
#                  EXCLUDE_FROM_ALL)

# if (CMAKE_VERSION VERSION_LESS 2.8.22)
#    include_directories("${gtest_SOURCE_DIR}/include")
# endif()

add_subdirectory(third/gtest/googletest)

set(CDT_ROOT_DIR ${CMAKE_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/dipc.cdt-config.cmake ${CMAKE_BINARY_DIR}/lib/cmake/dipc.cdt/dipc.cdt-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/DipcCDTMacros.cmake ${CMAKE_BINARY_DIR}/lib/cmake/dipc.cdt/DipcCDTMacros.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/DipcWasmToolchain.cmake ${CMAKE_BINARY_DIR}/lib/cmake/dipc.cdt/DipcWasmToolchain.cmake @ONLY)

set(CDT_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/dipc.cdt-config.cmake ${CMAKE_BINARY_DIR}/CMakeModules/dipc.cdt-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeModules/dipc.cdt-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/dipc.cdt)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/DipcCDTMacros.cmake ${CMAKE_BINARY_DIR}/CMakeModules/DipcCDTMacros.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeModules/DipcCDTMacros.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/dipc.cdt)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/DipcWasmToolchain.cmake ${CMAKE_BINARY_DIR}/CMakeModules/DipcWasmToolchain.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeModules/DipcWasmToolchain.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/dipc.cdt)

set(CDT_ROOT_DIR "_PREFIX_")
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/dipc.cdt-config.cmake ${CMAKE_BINARY_DIR}/CMakeModules/dipc.cdt-config.cmake.package)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/DipcCDTMacros.cmake ${CMAKE_BINARY_DIR}/CMakeModules/DipcCDTMacrosPackage.cmake)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/DipcWasmToolchain.cmake ${CMAKE_BINARY_DIR}/CMakeModules/DipcWasmToolchainPackage.cmake)


include(CMakeModules/LibrariesExternalProject.cmake)
include(CMakeModules/InstallCDT.cmake)


# uninstall target 
if(NOT TARGET uninstall)
    configure_file(
      ${CMAKE_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake ${CMAKE_BINARY_DIR}/CMakeModules/cmake_uninstall.cmake IMMEDIATE @ONLY
    )
    add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/CMakeModules/cmake_uninstall.cmake)
endif()


add_subdirectory(gtest)
