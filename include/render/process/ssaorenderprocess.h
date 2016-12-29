#ifndef SSAOPROCESS_H
#define SSAOPROCESS_H

#include "openglincludes.h"

#include "renderprocess.h"
#include "shader.h"
#include "framebuffer.h"

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);

class Quad;

class SSAORenderProcess : public RenderProcess
{
public:
    SSAORenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);

    virtual void resize(const GLuint &width, const GLuint &height);

    void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    virtual void connect(RenderProcess *previous_process);

    //  Setters
    virtual void setActivated(const GLboolean &activated);

private:
    void generateNoise();

    Framebuffer m_SSAO_buffer;
    Framebuffer m_SSAO_blur_buffer;
    Shader m_SSAO_shader;
    Shader m_SSAO_blur_shader;
    Texture m_noise_texture;
    std::vector<glm::vec3> m_SSAO_kernel;

    GLuint m_num_samples;
    GLint *m_samples_locations;
    GLint m_projection_location;
    GLint m_window_size_location;
};

#endif // SSAOPROCESS_H
