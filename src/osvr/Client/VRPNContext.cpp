/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "VRPNContext.h"
#include "display_json.h"
#include "RouterPredicates.h"
#include "RouterTransforms.h"
#include "VRPNTrackerRouter.h"
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/ClientInterface.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    RouterEntry::~RouterEntry() {}

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string contextDevice = "OSVR@" + m_host;
        m_conn = vrpn_get_connection_by_name(contextDevice.c_str());

        /// @todo this is hardcoded routing, and not well done - just a stop-gap
        /// measure.
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands/left", TrackerSensorPredicate(0),
                           HydraTrackerTransform());
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands/right", TrackerSensorPredicate(1),
                           HydraTrackerTransform());
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands", AlwaysTruePredicate(),
                           HydraTrackerTransform());

        m_addTrackerRouter(
            "org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0",
            "/me/head", TrackerSensorPredicate(1));

        setParameter("/display",
                     std::string(reinterpret_cast<char *>(display_json),
                                 display_json_len));
    }

    VRPNContext::~VRPNContext() {}

    void VRPNContext::m_update() {
        // mainloop the VRPN connection.
        m_conn->mainloop();
        // Process each of the routers.
        for (auto const &p : m_routers) {
            (*p)();
        }
    }

    template <typename Predicate>
    void VRPNContext::m_addTrackerRouter(const char *src, const char *dest,
                                         Predicate pred) {
        m_addTrackerRouter(src, dest, pred, NullTrackerTransform());
    }

    template <typename Predicate, typename Transform>
    void VRPNContext::m_addTrackerRouter(const char *src, const char *dest,
                                         Predicate pred, Transform xform) {
        OSVR_DEV_VERBOSE("Adding tracker route for " << dest);
        m_routers.emplace_back(new VRPNTrackerRouter<Predicate, Transform>(
            this, m_conn.get(), src, dest, pred, xform));
    }

} // namespace client
} // namespace osvr