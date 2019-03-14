//
//  VROPoseFilterEuro.cpp
//  ViroKit
//
//  Created by Raj Advani on 3/1/19.
//  Copyright © 2019 Viro Media. All rights reserved.
//

#include "VROPoseFilterEuro.h"
#include "VROTime.h"
#include "VROLog.h"
#include <cmath>
#include <ctime>

// To tune these values:
// To minimize jitter and lag when tracking human motion, the two parameters (fcmin and beta)
// can be set using a simple two-step procedure. First beta is set to 0 and fcmin (mincutoff)
// to a reasonable middle-ground value such as 1 Hz. Then the body part is held steady or moved
// at a very low speed while fcmin is adjusted to remove jitter and preserve an acceptable lag
// during these slow movements (decreasing fcmin reduces jitter but increases lag, fcmin must be > 0).
// Next, the body part is moved quickly in different directions while beta is increased with a
// focus on minimizing lag. First find the right order of magnitude to tune beta, which depends
// on the kind of data you manipulate and their units: do not hesitate to start with values like
// 0.001 or 0.0001. You can first multiply and divide beta by factor 10 until you notice an effect
// on latency when moving quickly.
//
// Note that parameters fcmin and beta have clear conceptual relationships: if high speed lag is
// a problem, increase beta; if slow speed jitter is a problem, decrease fcmin.
static const double kEuroBeta = 1.0;
static const double kEuroFCMin = 1.7;

// -----------------------------------------------------------------

VROPoseFilterEuro::VROPoseFilterEuro(float trackingPeriodMs, float confidenceThreshold) :
    VROPoseFilter(trackingPeriodMs, confidenceThreshold) {
    
    double frequency = 60;
    double dcutoff = 1.0;
    for (int i = 0; i < kNumBodyJoints; i++) {
        _filters.push_back(std::make_shared<VROOneEuroFilter>(frequency, kEuroFCMin, kEuroBeta, dcutoff));
    }
}

VROPoseFilterEuro::~VROPoseFilterEuro() {

}

VROPoseFrame VROPoseFilterEuro::temporalFilter(const std::vector<VROPoseFrame> &frames, const VROPoseFrame &combinedFrame,
                                               const VROPoseFrame &newFrame) {
    VROPoseFrame dampenedJoints = newPoseFrame();
    for (int i = 0; i < kNumBodyJoints; i++) {
        VROBodyJointType type = (VROBodyJointType) i;
        
        // Compute filtered position
        const std::vector<VROInferredBodyJoint> &samples = newFrame[i];
        if (samples.empty()) {
            continue;
        }
        const VROInferredBodyJoint &sample = samples.front();
        
        VROVector3f filtered = _filters[i]->filter(sample.getCenter(), sample.getCreationTime() / 1000.0,
                                                   false);
        
        // Compute aggregate confidence
        const std::vector<VROInferredBodyJoint> &confidenceSamples = combinedFrame[i];
        if (confidenceSamples.empty()) {
            continue;
        }
        float sumConfidence = 0;
        for (int i = 0; i < confidenceSamples.size(); i++) {
            sumConfidence += confidenceSamples[i].getConfidence();
        }
        
        VROInferredBodyJoint dampenedJoint(type);
        dampenedJoint.setCenter(filtered);
        dampenedJoint.setConfidence(sumConfidence / (float) confidenceSamples.size());
        dampenedJoints[i] = { dampenedJoint };
    }
    
    return dampenedJoints;
}
