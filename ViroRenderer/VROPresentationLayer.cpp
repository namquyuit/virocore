//
//  VROPresentationLayer.cpp
//  ViroRenderer
//
//  Created by Raj Advani on 10/21/15.
//  Copyright © 2015 Viro Media. All rights reserved.
//

#include "VROPresentationLayer.h"
#include "VROLayerSubstrateMetal.h"
#include "VROLog.h"
#include "VROMath.h"

VROPresentationLayer::VROPresentationLayer(const VROLayer *layer) :
    VROLayer(this),
    _model(layer),
    _substrate(nullptr) {
    
}

VROPresentationLayer::~VROPresentationLayer() {
    delete (_substrate);
}

void VROPresentationLayer::setContents(const void *data, size_t dataLength, int width, int height) {
    _substrate->setContents(data, dataLength, width, height);
}

void VROPresentationLayer::hydrate(const VRORenderContext &context) {
    // TODO assert not hydrating twice!
    
    _substrate = new VROLayerSubstrateMetal(shared_from_this());
    _substrate->hydrate(context);
}

void VROPresentationLayer::render(const VRORenderContext &context, std::stack<matrix_float4x4> mvStack) {
    std::shared_ptr<VROLayer> superlayer = getSuperlayer();
    
    updateAnimatedFrame();
    
    VROPoint pt(_frame.origin.x + _frame.size.width  / 2.0f,
                _frame.origin.y + _frame.size.height / 2.0f,
                _frame.origin.z);
    
    matrix_float4x4 scaleMtx = matrix_from_scale(_frame.size.width, _frame.size.height, 1.0);
    
    /*
     If the layer is a sublayer, then its coordinate system follows the 2D
     convention of origin top-left, Y down.
     */
    float y = superlayer ? -pt.y : pt.y;
    matrix_float4x4 translationMtx = matrix_from_translation(pt.x, y, pt.z);
    matrix_float4x4 modelMtx = matrix_multiply(translationMtx, scaleMtx);
    
    matrix_float4x4 mvParent = mvStack.top();
    matrix_float4x4 mv = matrix_multiply(mvParent, modelMtx);
    
    _substrate->render(context, mv);
    
    /*
     Now render the children. The children are all transformed to the parent's origin (its top
     left corner).
     */
    float parentOriginY = superlayer ? -_frame.origin.y : _frame.origin.y + _frame.size.height;
    matrix_float4x4 childTransform = matrix_from_translation(_frame.origin.x, parentOriginY, _frame.origin.z);
    
    mvStack.push(matrix_multiply(mvStack.top(), childTransform));
    
    for (std::shared_ptr<VROLayer> childLayer : _sublayers) {
        childLayer->render(context, mvStack);
    }
    mvStack.pop();
}

void VROPresentationLayer::startFrameAnimation() {
    _animationStartFrame = _frame;
    
    _animation = VROAnimation::get();
    if (!_animation) {
        VROAnimation::beginImplicitAnimation();
        _animation = VROAnimation::get();
    }
    
    passert (_animation);
}

void VROPresentationLayer::updateAnimatedFrame() {
    if (_animation) {
        VRORect animationTargetFrame = _model->getFrame();
        float t = _animation->getT();
        
        float x = _animationStartFrame.origin.x + (animationTargetFrame.origin.x - _animationStartFrame.origin.x) * t;
        float y = _animationStartFrame.origin.y + (animationTargetFrame.origin.y - _animationStartFrame.origin.y) * t;
        float z = _animationStartFrame.origin.z + (animationTargetFrame.origin.z - _animationStartFrame.origin.z) * t;
        float width = _animationStartFrame.size.width + (animationTargetFrame.size.width - _animationStartFrame.size.width) * t;
        float height = _animationStartFrame.size.height + (animationTargetFrame.size.height - _animationStartFrame.size.height) * t;
        
        _frame = VRORectMake(x, y, z, width, height);
    }
}

void VROPresentationLayer::setFrame(VRORect frame) {
    if (_substrate) {
        startFrameAnimation();
    }
    else {
        VROLayer::setFrame(frame);
    }
}

void VROPresentationLayer::setBounds(VRORect bounds) {
    if (_substrate) {
        startFrameAnimation();
    }
    else {
        VROLayer::setBounds(bounds);
    }
}

void VROPresentationLayer::setPosition(VROPoint point) {
    if (_substrate) {
        startFrameAnimation();
    }
    else {
        VROLayer::setPosition(point);
    }
}

