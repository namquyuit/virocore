//
//  VROTextTest.h
//  ViroKit
//
//  Created by Raj Advani on 10/1/17.
//  Copyright © 2017 Viro Media. All rights reserved.
//

#ifndef VROTextTest_h
#define VROTextTest_h

#include "VRORendererTest.h"

class VROTextTest;

class VROTextEventDelegate : public VROEventDelegate {
public:
    VROTextEventDelegate(VROTextTest *test) : _test(test) {};
    virtual ~VROTextEventDelegate() {};
    void onClick(int source, std::shared_ptr<VRONode> node, ClickState clickState, std::vector<float> position);
private:
    VROTextTest *_test;
};

class VROTextSample {
public:
    std::wstring sample;
    std::vector<std::shared_ptr<VROTypeface>> typefaces;
    VROTextSample(std::wstring sample, std::shared_ptr<VROTypeface> typeface) : sample(sample), typefaces({ typeface }) {}
    VROTextSample(std::wstring sample, std::vector<std::shared_ptr<VROTypeface>> typefaces) : sample(sample), typefaces(typefaces) {}
    ~VROTextSample() {}
};

class VROTextTest : public VRORendererTest {
public:
    
    VROTextTest();
    virtual ~VROTextTest();
    
    void build(std::shared_ptr<VRORenderer> renderer,
               std::shared_ptr<VROFrameSynchronizer> frameSynchronizer,
               std::shared_ptr<VRODriver> driver);
    std::shared_ptr<VRONode> getPointOfView() {
        return _pointOfView;
    }
    std::shared_ptr<VROSceneController> getSceneController() {
        return _sceneController;
    }
    
    void rotateText();

private:

    int _textIndex;
    std::vector<VROTextSample> _textSamples;
    std::shared_ptr<VRONode> _textNode;
    std::shared_ptr<VRONode> _pointOfView;
    std::shared_ptr<VROSceneController> _sceneController;
    std::shared_ptr<VROTextEventDelegate> _eventDelegate;
    
};

#endif /* VROTextTest_h */
