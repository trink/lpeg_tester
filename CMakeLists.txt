# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(lpeg_tester)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lua Parsing Expression Grammar Tester")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 8)

set(PROJECT_PATH "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(trink)
include(externals)

if(CMAKE_HOST_UNIX)
  find_library(PTHREAD_LIBRARY pthread)
  find_library(NSL_LIBRARY nsl)
  find_library(OPENSSL_LIBRARY ssl)
  find_library(RT_LIBRARY rt)
  find_library(LIBZ_LIBRARY z)
  set(UNIX_LIBRARIES
  ${PTHREAD_LIBRARY}
  ${NSL_LIBRARY}
  ${OPENSSL_LIBRARY}
  ${RT_LIBRARY}
  ${LIBZ_LIBRARY})
endif()

find_package(Boost 1.55.0 REQUIRED
date_time
filesystem
program_options
random regex
thread
signals
system unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Wt REQUIRED)
include_directories(${Wt_INCLUDE_DIR})

include_directories("${PROJECT_PATH}/include")

add_subdirectory(src)
install(DIRECTORY resource_bundle DESTINATION lpeg_tester)
install(DIRECTORY resources DESTINATION lpeg_tester)
install(DIRECTORY share DESTINATION lpeg_tester)
install(DIRECTORY "${CMAKE_BINARY_DIR}/ep_base/Source/lua_sandbox/modules/" DESTINATION lpeg_tester/modules)
install(FILES lpeg_tester_config.xml DESTINATION lpeg_tester)
if(WIN32)
  install(PROGRAMS run.bat DESTINATION lpeg_tester)
else()
  install(PROGRAMS run.sh DESTINATION lpeg_tester)
endif()

