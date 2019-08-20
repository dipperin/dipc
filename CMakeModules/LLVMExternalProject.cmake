message("cmake run LLVMExternalProject.cmake~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")

include(ExternalProject)
find_package(Git REQUIRED)
include(GNUInstallDirs)

set(EXTRA_CMAKE_ARGS)
list(APPEND EXTRA_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/llvm
  -DCMAKE_BUILD_TYPE=Release        #  ????
  #-DCMAKE_BUILD_TYPE=Debug        #  ????
  -DLLVM_TARGETS_TO_BUILD=X86
  -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly
  -DLLVM_INCLUDE_EXAMPLES=OFF
  -DLLVM_INCLUDE_TESTS=OFF
  -DCLANG_INCLUDE_TESTS=OFF
  -DLLVM_ENABLE_RTTI=ON)

if (WIN32)
  list(APPEND EXTRA_CMAKE_ARGS
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
    -DLLVM_ENABLE_ZLIB=OFF)
endif()

message("EXTRA_CMAKE_ARGS: ${EXTRA_CMAKE_ARGS}")
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")

ExternalProject_Add(
  DipcLLVM
  CMAKE_ARGS ${EXTRA_CMAKE_ARGS}    ## Arguments to CMake command line
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/dipc-llvm"   # Source dir to be used for build
  BINARY_DIR "${CMAKE_BINARY_DIR}/dipc-llvm"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1
  )
