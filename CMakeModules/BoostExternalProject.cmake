message("cmake run BoostExternalProject.cmake~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")

include(ExternalProject)
include(GNUInstallDirs)

set_property(DIRECTORY PROPERTY EP_BASE thirdparty)

set(BUILD_COMMAND_EXTRA)
if (WIN32)
  list(APPEND BUILD_COMMAND_EXTRA
    --layout=system
    address-model=64
    variant=release)
endif()

ExternalProject_Add(
  boost
  URL https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2

  CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=filesystem,system,log,thread,random,exception
  BUILD_COMMAND ./b2 link=static ${BUILD_COMMAND_EXTRA}
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1
  )

set(Boost_NO_SYSTEM_PATHS ON)