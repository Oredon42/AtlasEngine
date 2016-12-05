#ifndef SSAOPOSTPROCESS_H
#define SSAOPOSTPROCESS_H

#include "openglincludes.h"

#include "shader.h"
#include "framebuffer.h"

class Quad;

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);

class SSAOPostProcess
{
public:
    SSAOPostProcess(Quad &quad);

    void init(const GLuint &window_width, const GLuint &window_height);

    void process(const Framebuffer &gBuffer, const glm::mat4 &projection);

    //  Getters
    inline GLuint getTexture()const {return m_SSAO_buffer.getTexture(0);}

private:
    void generateNoise();

    Quad &m_quad;
    Framebuffer m_SSAO_buffer;
    Framebuffer m_SSAO_blur_buffer;
    Shader m_SSAO_shader;
    Shader m_SSAO_blur_shader;
    Texture m_noise_texture;
    std::vector<glm::vec3> m_SSAO_kernel;
};

#endif // SSAOPOSTPROCESS_H
