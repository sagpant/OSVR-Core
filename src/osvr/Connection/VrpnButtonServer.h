/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_VrpnButtonServer_h_GUID_264EFE6E_C328_45AE_B296_10980DD053AE
#define INCLUDED_VrpnButtonServer_h_GUID_264EFE6E_C328_45AE_B296_10980DD053AE

// Internal includes
#include "DeviceConstructionData.h"
#include <osvr/Connection/ButtonServerInterface.h>

// Library/third-party includes
#include <vrpn_Button.h>

// Standard includes
#include <cmath>

namespace osvr {
namespace connection {
    class VrpnButtonServer : public vrpn_Button_Filter,
                             public ButtonServerInterface {
      public:
        typedef vrpn_Button_Filter Base;
        VrpnButtonServer(DeviceConstructionData &init)
            : vrpn_Button_Filter(init.getQualifiedName().c_str(), init.conn) {
            m_setNumChannels(
                std::min(*init.obj.getAnalogs(),
                         OSVR_ChannelCount(vrpn_BUTTON_MAX_BUTTONS)));
            // Initialize data
            memset(Base::buttons, 0, sizeof(Base::buttons));
            memset(Base::lastbuttons, 0, sizeof(Base::lastbuttons));
        }

        virtual bool setValue(value_type val, OSVR_ChannelCount chan,
                              util::time::TimeValue const &timestamp) {
            if (chan >= m_getNumChannels()) {
                return false;
            }
            Base::buttons[chan] = val;
            m_reportChanges(timestamp);
            return true;
        }

        virtual void setValues(value_type val[], OSVR_ChannelCount chans,
                               util::time::TimeValue const &timestamp) {
            if (chans > m_getNumChannels()) {
                chans = m_getNumChannels();
            }
            for (OSVR_ChannelCount i = 0; i < chans; ++i) {
                Base::buttons[i] = val[i];
            }
            m_reportChanges(timestamp);
        }

      private:
        OSVR_ChannelCount m_getNumChannels() {
            return static_cast<OSVR_ChannelCount>(Base::num_buttons);
        }
        void m_setNumChannels(OSVR_ChannelCount chans) {
            Base::num_buttons = chans;
        }
        void m_reportChanges(util::time::TimeValue const &timestamp) {
            util::time::toStructTimeval(Base::timestamp, timestamp);
            Base::report_changes();
        }
    };

} // namespace connection
} // namespace osvr

#endif // INCLUDED_VrpnButtonServer_h_GUID_264EFE6E_C328_45AE_B296_10980DD053AE
