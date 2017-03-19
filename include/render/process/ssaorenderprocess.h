#ifndef SSAOPROCESS_H
#define SSAOPROCESS_H

#include "openglincludes.h"

#include "renderprocess.h"
#include "shader.h"
#include "framebuffer.h"

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);

class Quad;

/*
 * IN:
 * -RGBA : position + depth
 * -RGB  : normal
 *
 * OUT:
 * -RED  : ambient occlusion
 * */

class SSAORenderProcess : public QObject, public RenderProcess
{
    Q_OBJECT
public:
    SSAORenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement();

    virtual void resize(const GLuint &width, const GLuint &height);
    void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Setters
    virtual void setActivated(const GLboolean &activated);

private slots:
    void switchSSAO();

private:
    void generateNoise();

    GLboolean m_SSAO;

    Framebuffer m_SSAO_buffer;
    Framebuffer m_SSAO_blur_buffer;
    Shader m_SSAO_shader;
    Shader m_SSAO_blur_shader;
    Texture m_noise_texture;
    std::vector<glm::vec3> m_SSAO_kernel;

    GLuint m_num_samples;
    GLint *m_samples_locations;
    GLint m_view_location;
    GLint m_transposed_inverse_view_location;
    GLint m_projection_location;
    GLint m_window_size_location;
};

#endif // SSAOPROCESS_H
