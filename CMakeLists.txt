cmake_minimum_required(VERSION 3.5)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 1)
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
