#ifndef GEOMETRYPROCESS_H
#define GEOMETRYPROCESS_H

#include "a_renderprocess.h"

class Scene;
class Shader;
class Model;

/*
 * OUT:
 * -RGBA : position + depth
 * -RGB  : normal
 * -RGBA : albedo + specular
 * -RGB  : material (roughness + ior + metalness)
 * */
class GeometryRenderProcess : public ARenderProcess
{
private:
    enum GeometryPassShaderTypeFlags
    {
        AlbedoMap = 0x1,
        MetalnessMap = 0x2,
        RoughnessMap = 0x4,
        RefractionMap = 0x8,
        OpacityMap = 0x10,
        NormalMap = 0x20,
        Animated = 0x40
    };
    static const unsigned int s_uiNumShaderTypes = AlbedoMap | MetalnessMap | RoughnessMap | RefractionMap | OpacityMap | NormalMap | Animated;

    struct GeometryPassShaderType
    {
        unsigned int uiFlags;

        GeometryPassShaderType(const int &uiFlags) : uiFlags(uiFlags) {}
        GeometryPassShaderType(const Model *pModel);

        inline bool hasAlbedoMap() const{return (uiFlags & AlbedoMap) != 0;}
        inline bool hasMetalnessMap() const{return (uiFlags & MetalnessMap) != 0;}
        inline bool hasRoughnessMap() const{return (uiFlags & RoughnessMap) != 0;}
        inline bool hasRefractionMap() const{return (uiFlags & RefractionMap) != 0;}
        inline bool hasOpacityMap() const{return (uiFlags & OpacityMap) != 0;}
        inline bool hasNormalMap() const{return (uiFlags & NormalMap) != 0;}
        inline bool isAnimated() const{return (uiFlags & Animated) != 0;}
    };

public:
    GeometryRenderProcess();

    virtual void init(const GLsizei &iWidth, const GLsizei &iHeight);

    virtual void resize(const GLsizei &iWidth, const GLsizei &iHeight);
    virtual void process(const float &fRenderTime);

private:
    Framebuffer *m_pGBuffer;

    Shader *m_aShaders;

    //  Private methods
    Shader *generateShaderFromType(const GeometryRenderProcess::GeometryPassShaderType &sShaderType);
};

#endif // GEOMETRYPROCESS_H
