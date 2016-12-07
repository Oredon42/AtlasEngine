#include <random>
#include "ssaopostprocess.h"
#include "include/quad.h"

SSAOPostProcess::SSAOPostProcess(Quad &quad) :
    m_quad(quad)
{

}

void SSAOPostProcess::init(const GLuint &window_width, const GLuint &window_height)
{
    m_SSAO_shader.init("shaders/ssao.vert", "shaders/ssao.frag");
    m_SSAO_blur_shader.init("shaders/ssao.vert", "shaders/ssaoblur.frag");
    m_SSAO_shader.use();
    glUniform1i(glGetUniformLocation(m_SSAO_shader.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_SSAO_shader.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_SSAO_shader.getProgram(), "texNoise"), 2);

    m_SSAO_buffer.init(window_width, window_height);
    FramebufferTextureDatas SSAO_texture_datas[1] = {//FramebufferTextureDatas(GL_RED, GL_RGB, GL_FLOAT)};
                                                     FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT)};
    m_SSAO_buffer.attachTextures(SSAO_texture_datas, 1);
    m_SSAO_blur_buffer.init(window_width, window_height);
    m_SSAO_blur_buffer.attachTextures(SSAO_texture_datas, 1);

    generateNoise();
}

void SSAOPostProcess::process(const Framebuffer &gBuffer, const glm::mat4 &projection)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_SSAO_buffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT);
    m_SSAO_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(2));

    // Send kernel + rotation
    for (GLuint i = 0; i < 64; ++i)
        glUniform3fv(glGetUniformLocation(m_SSAO_shader.getProgram(), ("samples[" + std::to_string(i) + "]").c_str()), 1, &m_SSAO_kernel[i][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_SSAO_shader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    m_quad.draw();


    glBindFramebuffer(GL_FRAMEBUFFER, m_SSAO_blur_buffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT);
    m_SSAO_blur_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SSAO_buffer.getTexture(0));
    m_quad.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOPostProcess::generateNoise()
{
    std::uniform_real_distribution<GLfloat> random_floats(0.0, 1.0);
    std::default_random_engine generator;

    for(GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, random_floats(generator));
        sample = glm::normalize(sample);
        sample *= random_floats(generator);
        GLfloat scale = GLfloat(i) / 64.0;

        // Scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_SSAO_kernel.push_back(sample);
    }

    // Noise texture
    std::vector<glm::vec3> ssao_noise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, 0.0f);
        ssao_noise.push_back(noise);
    }
    m_noise_texture.init(GL_RGB16F, 4, 4, GL_RGB, GL_FLOAT, &ssao_noise[0], GL_REPEAT, GL_NEAREST, GL_NEAREST);
}

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
    return a + f * (b - a);
}
