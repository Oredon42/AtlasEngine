#ifndef LIGHTINGPROCESS_H
#define LIGHTINGPROCESS_H

#include "renderprocess.h"
#include "framebuffer.h"
#include "shader.h"

class Scene;

class LightingRenderProcess : public RenderProcess
{
public:
    LightingRenderProcess(const GLuint &width, const GLuint &height, const GLuint nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);

    virtual void resize(const GLuint &width, const GLuint &height);
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Getters
    inline Shader getShader() const{return m_shader;}

private:
    Framebuffer m_buffer;
    Shader m_shader;
};

#endif // LIGHTINGPROCESS_H
