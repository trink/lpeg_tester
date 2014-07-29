# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include(ExternalProject)

set_property(DIRECTORY PROPERTY EP_BASE "${CMAKE_BINARY_DIR}/ep_base")

externalproject_add(
    "lua_sandbox"
    GIT_REPOSITORY https://github.com/mozilla-services/lua_sandbox.git
    GIT_TAG 53a4d5eb0bf96abbfb4fee0e82e0f55bf5e29575
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${PROJECT_PATH} -DLUA_JIT=off
    INSTALL_DIR ${PROJECT_PATH}
)

set(LUA_SANDBOX_LIBRARIES "${PROJECT_PATH}/lib/libluasandbox.a" "${PROJECT_PATH}/lib/libcjson.a" "${PROJECT_PATH}/lib/liblpeg.a" "${PROJECT_PATH}/lib/liblua.a" )
