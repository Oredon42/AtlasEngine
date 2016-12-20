#ifndef SSAOPOSTPROCESS_H
#define SSAOPOSTPROCESS_H

#include "openglincludes.h"

#include "shader.h"
#include "framebuffer.h"

enum PostProcessQuality
{
    LOW,
    MEDIUM,
    HIGH
};

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);

class Quad;

class SSAOPostProcess
{
public:
    SSAOPostProcess(Quad &quad);

    void init(const GLuint &window_width, const GLuint &window_height);
    void resize(const GLuint &width, const GLuint &height);

    void process(const Framebuffer &gBuffer, const glm::mat4 &projection);

    //  Getters
    inline GLuint getOutTexture()const {return m_out_texture;}

    //  Setters
    void setActivated(const GLboolean &activated);
    void setQuality(const PostProcessQuality &quality);

private:
    void generateNoise();

    GLuint m_width;
    GLuint m_height;

    Quad &m_quad;

    GLboolean m_activated;

    GLuint m_out_texture;

    Framebuffer m_SSAO_buffer;
    Framebuffer m_SSAO_blur_buffer;
    Shader m_SSAO_shader;
    Shader m_SSAO_blur_shader;
    Texture m_noise_texture;
    std::vector<glm::vec3> m_SSAO_kernel;
};

#endif // SSAOPOSTPROCESS_H
