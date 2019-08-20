add_custom_command(TARGET DipcLLVM POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)
macro(dipc_llvm_install file)
  set(BINARY_DIR ${CMAKE_BINARY_DIR}/dipc-llvm/bin)
  add_custom_command(TARGET DipcLLVM POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file}${BIN_SUFFIX} ${CMAKE_BINARY_DIR}/bin/${file}${BIN_SUFFIX})
  install(FILES ${BINARY_DIR}/${file}${BIN_SUFFIX}
    DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro(dipc_llvm_install)

macro(dipc_tools_install file)
  set(BINARY_DIR ${CMAKE_BINARY_DIR}/tools/bin)
  add_custom_command(TARGET DipcTools POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file}${BIN_SUFFIX} ${CMAKE_BINARY_DIR}/bin/${file}${BIN_SUFFIX})
  install(FILES ${BINARY_DIR}/${file}${BIN_SUFFIX}
    DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  message("CMAKE_INSTALL_FULL_BINDIR  ${CMAKE_INSTALL_FULL_BINDIR}  ")
endmacro(dipc_tools_install)

macro(dipc_libraries_install)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include)
   install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
   install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   install(DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/boost/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   install(DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/rapidjson/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   install(DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/dipclib/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   message("installCDT.cmake CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}  CMAKE_INSTALL_FULL_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}")
endmacro(dipc_libraries_install)

set(CLANG clang-7)
if (WIN32)
  set(CLANG clang)
endif()

dipc_llvm_install(${CLANG})
dipc_llvm_install(opt)
dipc_llvm_install(llc)
dipc_llvm_install(lld)
dipc_llvm_install(wasm-ld)
dipc_tools_install(dipc-cpp)
dipc_tools_install(dipc-ld)
dipc_tools_install(dipc-init)
dipc_tools_install(dipc-abigen)
dipc_libraries_install()
