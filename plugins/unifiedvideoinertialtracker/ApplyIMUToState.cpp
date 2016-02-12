/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "ApplyIMUToState.h"
#include "TrackingSystem.h"

// Library/third-party includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/AbsoluteOrientationMeasurement.h>
#include <osvr/Kalman/AngularVelocityMeasurement.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    inline void applyOriToState(TrackingSystem const &sys, BodyState &state,
                                BodyProcessModel &processModel,
                                CannedIMUMeasurement const &meas) {
        Eigen::Quaterniond quat;
        meas.restoreQuat(quat);
        Eigen::Vector3d var;
        meas.restoreQuatVariance(var);

        /// Rotate it into camera space
        /// @todo do this without rotating into camera space?
        Eigen::Quaterniond toCameraSpace(
            sys.getCameraPose().rotation().transpose());
        quat = toCameraSpace * quat;

        kalman::AbsoluteOrientationMeasurement<BodyState> kalmanMeas{quat, var};
        kalman::correct(state, processModel, kalmanMeas);
    }

    inline void applyAngVelToState(TrackingSystem const &sys, BodyState &state,
                                   BodyProcessModel &processModel,
                                   CannedIMUMeasurement const &meas) {

        Eigen::Vector3d angVel;
        meas.restoreAngVel(angVel);
        Eigen::Vector3d var;
        meas.restoreAngVelVariance(var);

        kalman::AngularVelocityMeasurement<BodyState> kalmanMeas{angVel, var};
        kalman::correct(state, processModel, kalmanMeas);
    }

    void applyIMUToState(TrackingSystem const &sys,
                         util::time::TimeValue const &initialTime,
                         BodyState &state, BodyProcessModel &processModel,
                         util::time::TimeValue const &newTime,
                         CannedIMUMeasurement const &meas) {
        if (newTime != initialTime) {
            auto dt = osvrTimeValueDurationSeconds(&newTime, &initialTime);
            kalman::predict(state, processModel, dt);
        }
        if (meas.orientationValid()) {
            applyOriToState(sys, state, processModel, meas);
        } else if (meas.angVelValid()) {
/// @todo handle angular velocity transforms!
#if 0
            applyAngVelToState(sys, state, processModel, meas);
#endif
        } else {
            // unusually, the measurement is totally invalid. Just normalize and
            // go on.
            state.postCorrect();
        }
    }
} // namespace vbtracker
} // namespace osvr
