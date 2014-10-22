/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "PluginSpecificRegistrationContext.h"
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none
namespace ogvr {
PluginSpecificRegistrationContext::PluginSpecificRegistrationContext(
    std::string const &name)
    : m_name(name) {
    OGVR_DEV_VERBOSE("Creating a plugin registration context for " << m_name);
}

PluginSpecificRegistrationContext::~PluginSpecificRegistrationContext() {
    /// @todo run deleter callbacks in reverse order of registration.

    OGVR_DEV_VERBOSE("Destroying plugin reg context: Here's where we'd call "
                     "deleter callbacks for "
                     << m_name);
}

} // end of namespace ogvr
