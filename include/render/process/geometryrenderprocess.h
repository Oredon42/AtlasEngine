#ifndef GEOMETRYPROCESS_H
#define GEOMETRYPROCESS_H

#include "renderprocess.h"
#include "shader.h"
#include "framebuffer.h"

class Scene;

class GeometryRenderProcess : public RenderProcess
{
public:
    GeometryRenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);

    virtual void resize(const GLuint &width, const GLuint &height);
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    inline Shaders getShaders(){return m_shaders;}

private:
    Framebuffer m_gBuffer;
    GLuint m_rbo_depth;

    Shaders m_shaders;
    ShaderTypes m_shader_types;
};

#endif // GEOMETRYPROCESS_H
