//
//  VROARSessionARCore.cpp
//  ViroKit
//
//  Created by Raj Advani on 9/27/17.
//  Copyright © 2017 Viro Media. All rights reserved.
//

#include "VROARSessionARCore.h"
#include "VROARFrameARCore.h"
#include "VROARAnchor.h"
#include "VROARPlaneAnchor.h"
#include "VROTexture.h"
#include "VRODriver.h"
#include "VROScene.h"
#include "VROTextureSubstrate.h"
#include "VROLog.h"
#include <algorithm>
#include "VROPlatformUtil.h"

VROARSessionARCore::VROARSessionARCore(jni::Object<arcore::Session> sessionJNI, std::shared_ptr<VRODriver> driver) :
    VROARSession(VROTrackingType::DOF6) {

    _sessionJNI = sessionJNI.NewGlobalRef(*VROPlatformGetJNIEnv());
    _background = std::make_shared<VROTexture>(VROTextureType::Texture2D, VROTextureInternalFormat::YCBCR);
}

VROARSessionARCore::~VROARSessionARCore() {
 
}

#pragma mark - VROARSession implementation

void VROARSessionARCore::run() {
    // TODO We can make this resume(), but on Android this is controlled externally
    //      by way of the activity lifecycle (we invoke pause and resume upon receiving
    //      lifecycle callbacks
}

void VROARSessionARCore::pause() {
    arcore::session::pause(*_sessionJNI.get());
}

bool VROARSessionARCore::isReady() const {
    return true;
    // TODO
    //return getScene() != nullptr && _currentFrame.get() != nullptr;
}

void VROARSessionARCore::setAnchorDetection(std::set<VROAnchorDetection> types) {
    //TODO VIRO-1895
}

void VROARSessionARCore::setScene(std::shared_ptr<VROScene> scene) {
    VROARSession::setScene(scene);
}

void VROARSessionARCore::addAnchor(std::shared_ptr<VROARAnchor> anchor) {
    std::shared_ptr<VROARSessionDelegate> delegate = getDelegate();
    if (!delegate) {
        return;
    }
    
    delegate->anchorWasDetected(anchor);
    _anchors.push_back(anchor);
}

void VROARSessionARCore::removeAnchor(std::shared_ptr<VROARAnchor> anchor) {
    _anchors.erase(std::remove_if(_anchors.begin(), _anchors.end(),
                                 [anchor](std::shared_ptr<VROARAnchor> candidate) {
                                     return candidate == anchor;
                                 }), _anchors.end());

    //TODO VIRO-1895
    /*
    for (auto it = _nativeAnchorMap.begin(); it != _nativeAnchorMap.end();) {
        if (it->second == anchor) {
            // TODO We should remove the anchor from the ARKit session, but unclear
            //      how to do this given just the identifier. Do we create a dummy
            //      ARAnchor and set its identifier?
            //[_session removeAnchor:it->first];
            it = _nativeAnchorMap.erase(it);
        }
        else {
            ++it;
        }
    }
     */
    
    std::shared_ptr<VROARSessionDelegate> delegate = getDelegate();
    if (delegate) {
        delegate->anchorWasRemoved(anchor);
    }
}

void VROARSessionARCore::updateAnchor(std::shared_ptr<VROARAnchor> anchor) {
    std::shared_ptr<VROARSessionDelegate> delegate = getDelegate();
    if (delegate) {
        delegate->anchorWillUpdate(anchor);
    }
    anchor->updateNodeTransform();
    if (delegate) {
        delegate->anchorDidUpdate(anchor);
    }
}

std::shared_ptr<VROTexture> VROARSessionARCore::getCameraBackgroundTexture() {
    return _background;
}

std::unique_ptr<VROARFrame> &VROARSessionARCore::updateFrame() {
    // TODO Set the actual background texture
    arcore::session::setCameraTextureName( *_sessionJNI.get(), 0);
    jni::Object<arcore::Frame> frameJNI = arcore::session::update(*_sessionJNI.get());
    _currentFrame = std::make_unique<VROARFrameARCore>(frameJNI, _viewport, shared_from_this());
    return _currentFrame;
}

std::unique_ptr<VROARFrame> &VROARSessionARCore::getLastFrame() {
    return _currentFrame;
}

void VROARSessionARCore::setViewport(VROViewport viewport) {
    _viewport = viewport;
}

void VROARSessionARCore::setOrientation(VROCameraOrientation orientation) {
    _orientation = orientation;
}

#pragma mark - Internal Methods

VROMatrix4f VROARSessionARCore::getProjectionMatrix(float near, float far) {
    return arcore::session::getProjectionMatrix(*_sessionJNI.get(), near, far);
}