//
//  VROARSessionARCore.h
//  ViroKit
//
//  Created by Raj Advani on 9/27/17.
//  Copyright © 2017 Viro Media. All rights reserved.
//

#ifndef VROARSessioniOS_h
#define VROARSessioniOS_h

#include "VROARSession.h"
#include "VROARFrameARCore.h"
#include "VROViewport.h"
#include "VROOpenGL.h"
#include "arcore/ARCore_JNI.h"
#include <map>
#include <vector>
#include <VROCameraTexture.h>
#include <VROARPlaneAnchor.h>

class VRODriverOpenGL;

class VROARSessionARCore : public VROARSession, public std::enable_shared_from_this<VROARSessionARCore> {
public:
    
    VROARSessionARCore(jni::Object<arcore::Session> sessionJNI,
                       jni::Object<arcore::ViroViewARCore> viroViewJNI,
                       std::shared_ptr<VRODriverOpenGL> driver);
    virtual ~VROARSessionARCore();

    void run();
    void pause();
    bool isReady() const;
    void resetSession(bool resetTracking, bool removeAnchors);
    
    void setScene(std::shared_ptr<VROScene> scene);
    void setDelegate(std::shared_ptr<VROARSessionDelegate> delegate);
    void setAnchorDetection(std::set<VROAnchorDetection> types);
    void addARImageTarget(std::shared_ptr<VROARImageTarget> target);
    void removeARImageTarget(std::shared_ptr<VROARImageTarget> target);
    void addAnchor(std::shared_ptr<VROARAnchor> anchor);
    void removeAnchor(std::shared_ptr<VROARAnchor> anchor);
    void updateAnchor(std::shared_ptr<VROARAnchor> anchor);

    std::unique_ptr<VROARFrame> &updateFrame();
    std::unique_ptr<VROARFrame> &getLastFrame();
    std::shared_ptr<VROTexture> getCameraBackgroundTexture();
    
    void setViewport(VROViewport viewport);
    void setOrientation(VROCameraOrientation orientation);
    void setWorldOrigin(VROMatrix4f relativeTransform);

    GLuint getCameraTextureId() const;
    
    /*
     Internal methods.
     */
    void initGL(std::shared_ptr<VRODriverOpenGL> driver);
    std::shared_ptr<VROARAnchor> getAnchorForNative(jni::Object<arcore::Anchor> anchor);

private:
    /*
     The ARCore session.
     */
    jni::UniqueWeakObject<arcore::Session> _sessionJNI;

    /*
     The ViroViewARCore object.
     */
    jni::UniqueWeakObject<arcore::ViroViewARCore> _viroViewJNI;

    /*
     The last computed ARFrame.
     */
    std::unique_ptr<VROARFrame> _currentFrame;
    
    /*
     The current viewport and camera orientation.
     */
    VROViewport _viewport;
    VROCameraOrientation _orientation;

    /*
     Vector of all anchors that have been added to this session.
     */
    std::vector<std::shared_ptr<VROARAnchor>> _anchors;

    arcore::config::LightingMode _lightingMode;
    arcore::config::PlaneFindingMode _planeFindingMode;
    arcore::config::UpdateMode  _updateMode;

    /*
     Map of ARCore anchors ("native" anchors) to their Viro representation.
     Required so we can update VROARAnchors when their ARCore counterparts are
     updated.
     */
    std::map<std::string, std::shared_ptr<VROARAnchor>> _nativeAnchorMap;

    /*
     Background to be assigned to the VROScene.
     */
    std::shared_ptr<VROTexture> _background;

    /*
     The GL_TEXTURE_EXTERNAL_OES texture used for the camera background.
     */
    GLuint _cameraTextureId;

    void updateARCoreConfig();
    void processUpdatedAnchors(VROARFrameARCore *frame);
    void updateAnchorFromJni(std::shared_ptr<VROARAnchor> anchor, jni::Object<arcore::Anchor> anchorJni);
    void updatePlaneFromJni(std::shared_ptr<VROARPlaneAnchor> plane, jni::Object<arcore::Plane> planeJni);
};

#endif /* VROARSessionARCore_h */