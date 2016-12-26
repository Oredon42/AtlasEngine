#include "include/render/process/lightingrenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

LightingRenderProcess::LightingRenderProcess(const GLuint &width, const GLuint &height, const GLuint nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights) :
    RenderProcess(width, height)
{
    m_buffer.init(width, height);
    std::vector<FramebufferTextureDatas> lighting_textures_datas;
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER));
    m_buffer.attachTextures(lighting_textures_datas);

    m_shader.initLighting(nb_dirlights, nb_pointlights, nb_spotlights);
    m_shader.use();
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "AO"), 3);
    glUseProgram(0);

    m_out_textures.push_back(m_buffer.getTexture(0));
    m_out_textures.push_back(m_buffer.getTexture(1));
    m_out_textures.push_back(m_buffer.getTexture(2));
}

void LightingRenderProcess::resize(const GLuint &width, const GLuint &height)
{

}

void LightingRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    m_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(2));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(3));

    scene.sendViewSpacePointLightDatas(m_shader);
    scene.sendCameraToShader(m_shader, m_buffer.width(), m_buffer.height());

    quad.draw();
}
