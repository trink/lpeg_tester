/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/// @brief Prevent duplication of string constants between compilation units @file

#include "@CMAKE_CURRENT_SOURCE_DIR@/constants.h"

namespace trink {
namespace lpeg_tester {
const unsigned kVersionMajor = @CPACK_PACKAGE_VERSION_MAJOR@;
const unsigned kVersionMinor = @CPACK_PACKAGE_VERSION_MINOR@;
const unsigned kVersionPatch = @CPACK_PACKAGE_VERSION_PATCH@;

const std::string kProgramName("@PROJECT_NAME@");
const std::string kProgramDescription("@CPACK_PACKAGE_DESCRIPTION_SUMMARY@");

const std::string kWtConfig("@WT_CONFIG@");
}
}
