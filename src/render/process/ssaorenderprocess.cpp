#include <random>
#include "include/render/process/ssaorenderprocess.h"
#include "include/data/quad.h"
#include "include/data/scene.h"

SSAORenderProcess::SSAORenderProcess(const GLuint &width, const GLuint &height) :
    RenderProcess(width, height),
    m_num_samples(64)
{    
    m_SSAO_shader.init("shaders/ssao.vert", "shaders/ssao.frag");
    m_SSAO_blur_shader.init("shaders/ssao.vert", "shaders/ssaoblur.frag");
    m_SSAO_shader.use();
    glUniform1i(glGetUniformLocation(m_SSAO_shader.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_SSAO_shader.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_SSAO_shader.getProgram(), "texNoise"), 2);

    std::vector<FramebufferTextureDatas> SSAO_texture_datas;
    SSAO_texture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT));
    m_SSAO_buffer.init(width, height);
    m_SSAO_buffer.attachTextures(SSAO_texture_datas);
    m_SSAO_blur_buffer.init(width, height);
    m_SSAO_blur_buffer.attachTextures(SSAO_texture_datas);

    //m_SSAO_buffer.resize(m_width/2, m_height/2);

    m_samples_locations = new GLint[m_num_samples];
    for (GLuint i = 0; i < m_num_samples; ++i)
        m_samples_locations[i] = glGetUniformLocation(m_SSAO_shader.getProgram(), ("samples[" + std::to_string(i) + "]").c_str());

    m_projection_location = glGetUniformLocation(m_SSAO_shader.getProgram(), "projection");
    m_window_size_location = glGetUniformLocation(m_SSAO_shader.getProgram(), "window_size");


    generateNoise();
}

void SSAORenderProcess::resize(const GLuint &width, const GLuint &height)
{
    /*RenderProcess::resize(width, height);

    m_SSAO_buffer.resize(m_width, m_height);
    m_SSAO_blur_buffer.resize(m_width, m_height);*/
}

void SSAORenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    glViewport(0, 0, m_SSAO_buffer.width(), m_SSAO_buffer.height());
    glBindFramebuffer(GL_FRAMEBUFFER, m_SSAO_buffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT);

    m_SSAO_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_noise_texture.getId());

    for (GLuint i = 0; i < m_num_samples; ++i)
        glUniform3fv(m_samples_locations[i], 1, &m_SSAO_kernel[i][0]);
    glUniformMatrix4fv(m_projection_location, 1, GL_FALSE, glm::value_ptr(scene.getCurrentCamera()->getProjection()));
    glUniform2f(m_window_size_location, m_SSAO_buffer.width(), m_SSAO_buffer.height());
    quad.draw();


    glViewport(0, 0, m_SSAO_blur_buffer.width(), m_SSAO_blur_buffer.height());
    glBindFramebuffer(GL_FRAMEBUFFER, m_SSAO_blur_buffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT);

    m_SSAO_blur_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SSAO_buffer.getTexture(0));
    quad.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAORenderProcess::generateNoise()
{
    std::uniform_real_distribution<GLfloat> random_floats(0.0, 1.0);
    std::default_random_engine generator;

    //  Samples
    for(GLuint i = 0; i < m_num_samples; ++i)
    {
        glm::vec3 sample(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, random_floats(generator));
        sample = glm::normalize(sample);
        sample *= random_floats(generator);
        GLfloat scale = GLfloat(i) / GLfloat(m_num_samples);

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

void SSAORenderProcess::connect(RenderProcess *previous_process)
{
    RenderProcess::connect(previous_process);

    m_out_textures.push_back(m_previous_process->getOutTexture(0));
    m_out_textures.push_back(m_previous_process->getOutTexture(1));
    m_out_textures.push_back(m_previous_process->getOutTexture(2));
    m_out_textures.push_back(m_SSAO_blur_buffer.getTexture(0));
}

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
    return a + f * (b - a);
}

void SSAORenderProcess::setActivated(const GLboolean &activated)
{
    m_activated = activated;

    m_SSAO_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
