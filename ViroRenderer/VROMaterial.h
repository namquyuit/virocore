//
//  VROMaterial.h
//  ViroRenderer
//
//  Created by Raj Advani on 11/17/15.
//  Copyright © 2015 Viro Media. All rights reserved.
//

#ifndef VROMaterial_h
#define VROMaterial_h

#include <memory>
#include "VROMaterialVisual.h"
#include "VROAnimatable.h"

enum class VROCullMode {
    Back,
    Front,
    None
};

enum class VROBlendMode {
    Alpha,
    Add,
    Subtract,
    Multiply,
    Screen,
    Replace
};

enum class VROTransparencyMode {
    AOne,
    RGBZero
};

enum class VROLightingModel {
    Phong,
    Blinn,
    Lambert,
    Constant
};

/*
 Manages the lighting and shading attributes associated with the surface of a geometry that
 define its appearance when rendered. When you create a material, you define a collection of
 visual attributes and their options, which you can then reuse for multiple geometries 
 in a scene.
 */
class VROMaterial : public VROAnimatable {
    
public:
    
    VROMaterial();
    virtual ~VROMaterial();
    
    /*
     Copy constructor for this material. Texture contents use shared references.
     */
    VROMaterial(std::shared_ptr<VROMaterial> material);
    
    VROMaterialVisual &getDiffuse() {
        return *_diffuse;
    }
    VROMaterialVisual &getSpecular() {
        return *_specular;
    }
    VROMaterialVisual &getNormal() {
        return *_normal;
    }
    VROMaterialVisual &getReflective() {
        return *_reflective;
    }
    VROMaterialVisual &getEmission() {
        return *_emission;
    }
    VROMaterialVisual &getTransparent() {
        return *_transparent;
    }
    VROMaterialVisual &getMultiply() {
        return *_multiply;
    }
    VROMaterialVisual &getAmbientOcclusion() {
        return *_ambientOcclusion;
    }
    VROMaterialVisual &getSelfIllumination() {
        return *_selfIllumination;
    }
    
    void setShininess(float shininess);
    float getShininess() const {
        return _shininess;
    }
    
    void setFresnelExponent(float fresnelExponent);
    float getFresnelExponent() const {
        return _fresnelExponent;
    }
    
    void setTransparency(float transparency) {
        _transparency = transparency;
    }
    float getTransparency() const {
        return _transparency;
    }
    
    void setTransparencyMode(VROTransparencyMode mode) {
        _transparencyMode = mode;
    }
    VROTransparencyMode getTransparencyMode() const {
        return _transparencyMode;
    }
    
    void setLightingModel(VROLightingModel model) {
        _lightingModel = model;
    }
    VROLightingModel getLightingModel() const {
        return _lightingModel;
    }
    
    bool isLitPerPixel() const {
        return _litPerPixel;
    }
    VROCullMode getCullMode() const {
        return _cullMode;
    }
    VROBlendMode getBlendMode() const {
        return _blendMode;
    }
    bool getWritesToDepthBuffer() const {
        return _writesToDepthBuffer;
    }
    bool getReadsFromDepthBuffer() const {
        return _readsFromDepthBuffer;
    }
    
    void setWritesToDepthBuffer(bool writesToDepthBuffer) {
        _writesToDepthBuffer = writesToDepthBuffer;
    }
    void setReadsFromDepthBuffer(bool readsFromDepthBuffer) {
        _readsFromDepthBuffer = readsFromDepthBuffer;
    }
    
private:
    
    /*
     The visual properties associated with the material.
     */
    VROMaterialVisual *_diffuse;
    VROMaterialVisual *_specular;
    VROMaterialVisual *_normal;
    VROMaterialVisual *_reflective;
    VROMaterialVisual *_emission;
    VROMaterialVisual *_transparent;
    VROMaterialVisual *_multiply;
    VROMaterialVisual *_ambientOcclusion;
    VROMaterialVisual *_selfIllumination;
    
    /*
     User-provided name of the material.
     */
    std::string _name;
    
    /*
     The sharpness of specular highlights.
     */
    float _shininess;
    
    /*
     Factor affecting material reflectivity.
     */
    float _fresnelExponent;
    
    /*
     Uniform transparency of the material.
     */
    float _transparency;
    
    /*
     The mode used to calculate transparency.
     */
    VROTransparencyMode _transparencyMode;
    
    /*
     The lighting model to use to compute the interaction between
     the lights in the scene and this material's visual properties.
     */
    VROLightingModel _lightingModel;
    
    /*
     True means use per-pixel lighting, false means use per-vertex lighting.
     */
    bool _litPerPixel;
    
    /*
     Set to cull back faces, front faces, or none.
     */
    VROCullMode _cullMode;
    
    /*
     Determines how pixel colors rendered using this material blend with 
     pixel colors already in the render target.
     */
    VROBlendMode _blendMode;
    
    /*
     Depth write and read settings.
     */
    bool _writesToDepthBuffer, _readsFromDepthBuffer;
    
    /*
     Version of this material that's being animated away. Populated with the current
     values of this material whenever this material is changed.
     */
    std::shared_ptr<VROMaterial> _outgoing;
    
    /*
     The opacity of the outgoing material. When this hits 0, the outgoing material
     can be removed.
     */
    float _outgoingOpacity;
    
};

#endif /* VROMaterial_h */
