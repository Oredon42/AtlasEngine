#ifndef WIREFRAMEPROCESS_H
#define WIREFRAMEPROCESS_H

#include "src/core/openglincludes.h"

#include "src/core/render/process/renderprocess.h"
#include "src/core/data/shader.h"
#include "src/core/data/framebuffer.h"

class Quad;

/*
 * OUT:
 * -RGBA  : fragment color
 * */
class WireframeRenderProcess : public RenderProcess
{
public:
    WireframeRenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);

    virtual void resize(const GLuint &width, const GLuint &height);
    void process(const GLfloat &render_time);

    //  Setters
    virtual void setActivated(const GLboolean &activated);

private:
    void generateNoise();
    Framebuffer m_out_buffer;

    Shader m_color_shader;
};

#endif // WIREFRAMEPROCESS_H
