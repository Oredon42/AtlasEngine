#ifndef WIREFRAMEPROCESS_H
#define WIREFRAMEPROCESS_H

#include "openglincludes.h"

#include "renderprocess.h"
#include "shader.h"
#include "framebuffer.h"

class Quad;

class WireframeRenderProcess : public RenderProcess
{
public:
    WireframeRenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void resize(const GLuint &width, const GLuint &height);
    void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Setterss
    virtual void setActivated(const GLboolean &activated);

private:
    void generateNoise();
    Framebuffer m_out_buffer;

    Shader m_color_shader;
};

#endif // WIREFRAMEPROCESS_H
