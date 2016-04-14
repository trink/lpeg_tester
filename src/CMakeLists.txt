# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(LPEG_TESTER_SRC 
  constants.cpp 
  lpeg_tester.cpp)

if (Wt_CONNECTOR STREQUAL "isapi")
  set(WT_CONFIG "c:/witty/lpeg_tester_config.xml")
  add_library(lpeg_tester SHARED ${LPEG_TESTER_SRC})
  target_link_libraries(lpeg_tester ${LUASANDBOX_LIBRARIES} ${Wt_LIBRARIES} ${Boost_LIBRARIES} ws2_32.lib wldap32.lib)
  set_target_properties(lpeg_tester PROPERTIES LINK_FLAGS "/EXPORT:HttpExtensionProc /EXPORT:GetExtensionVersion /EXPORT:TerminateExtension")

  install(TARGETS tt DESTINATION bin)
else()
  add_executable(lpeg_tester ${LPEG_TESTER_SRC})
  target_link_libraries(lpeg_tester ${LUASANDBOX_LIBRARIES} ${Wt_LIBRARIES} ${Boost_LIBRARIES})
  if(CMAKE_HOST_UNIX)
    target_link_libraries(lpeg_tester ${UNIX_LIBRARIES})
  else()
    target_link_libraries(lpeg_tester ws2_32.lib wldap32.lib)
  endif()

  install(TARGETS lpeg_tester DESTINATION lpeg_tester)
endif()

configure_file(constants.in.cpp ${CMAKE_CURRENT_BINARY_DIR}/constants.cpp)
