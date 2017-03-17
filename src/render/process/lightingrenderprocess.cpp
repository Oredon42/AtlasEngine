#include "include/render/process/lightingrenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

LightingRenderProcess::LightingRenderProcess(const GLuint nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights) :
    RenderProcess(5)
{
    m_shader.initLightingPass(nb_dirlights, nb_pointlights, nb_spotlights);
}

void LightingRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(width, height);

    m_buffer.init(width, height);
    std::vector<FramebufferTextureDatas> lighting_textures_datas;
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER));
    m_buffer.attachTextures(lighting_textures_datas);

    m_shader.use();
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gMaterial"), 3);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "shadows"), 4);
    glUseProgram(0);

    m_out_textures.push_back(m_buffer.getTexture(0));
    m_out_textures.push_back(m_buffer.getTexture(1));
    m_out_textures.push_back(m_buffer.getTexture(2));
}

void LightingRenderProcess::initMenuElement()
{

}

void LightingRenderProcess::resize(const GLuint &width, const GLuint &height)
{
    RenderProcess::resize(width, height);

    m_buffer.resize(width, height);
}

void LightingRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    m_buffer.bind();
    glViewport(0, 0, m_buffer.width(), m_buffer.height());
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader.use();

    m_shader.activateNextTexture();
    bindPreviousTexture(0);
    m_shader.activateNextTexture();
    bindPreviousTexture(1);
    m_shader.activateNextTexture();
    bindPreviousTexture(2);
    m_shader.activateNextTexture();
    bindPreviousTexture(3);
    m_shader.activateNextTexture();
    //bindPreviousTexture(4);

    scene.sendLightDatas(m_shader);
    scene.sendCameraToShader(m_shader, m_buffer.width(), m_buffer.height());


    quad.draw();
}
