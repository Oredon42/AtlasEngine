#include "include/render/process/lightingrenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

LightingRenderProcess::LightingRenderProcess(const GLuint nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights) :
    RenderProcess(5, 3),
    m_2D_texture_depth_shader("shaders/2D_texture_depth.vert", "shaders/2D_texture_depth.frag"),
    m_cubemap_texture_depth_shader("shaders/cubemap_depth.vert", "shaders/cubemap_depth.frag", "shaders/cubemap_depth.geom")
{
    m_lighting_shader.initLightingPass(nb_dirlights, nb_pointlights, nb_spotlights);
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

    m_lighting_shader.use();
    glUniform1i(glGetUniformLocation(m_lighting_shader.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_lighting_shader.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_lighting_shader.getProgram(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(m_lighting_shader.getProgram(), "gMaterial"), 3);
    glUniform1i(glGetUniformLocation(m_lighting_shader.getProgram(), "AO"), 4);
    glUseProgram(0);

    m_out_textures[0] = m_buffer.getTexture(0);
    m_out_textures[1] = m_buffer.getTexture(1);
    m_out_textures[2] = m_buffer.getTexture(2);
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
    processShadows(scene, render_time, keys);

    m_buffer.bind();
    glViewport(0, 0, m_buffer.width(), m_buffer.height());
    glClear(GL_COLOR_BUFFER_BIT);
    m_lighting_shader.use();

    m_lighting_shader.activateNextTexture();
    bindPreviousTexture(0);
    m_lighting_shader.activateNextTexture();
    bindPreviousTexture(1);
    m_lighting_shader.activateNextTexture();
    bindPreviousTexture(2);
    m_lighting_shader.activateNextTexture();
    bindPreviousTexture(3);
    m_lighting_shader.activateNextTexture();
    bindPreviousTexture(4);

    scene.sendLightDatas(m_lighting_shader);
    scene.sendCameraToShader(m_lighting_shader, m_buffer.width(), m_buffer.height());


    quad.draw();
}

void LightingRenderProcess::processShadows(const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    //  DirLights
    for(size_t i = 0; i < scene.numberOfDirLights(); ++i)
    {
        DirLight *current_dirlight = scene.getDirLight(i);
        glViewport(0, 0, current_dirlight->getDepthMapWidth(), current_dirlight->getDepthMapHeight());
        current_dirlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_2D_texture_depth_shader.use();
        current_dirlight->sendShadowDatas(m_2D_texture_depth_shader);
        scene.drawNoLight(m_2D_texture_depth_shader, keys, render_time, current_dirlight->getDepthMapWidth(), current_dirlight->getDepthMapHeight());

        current_dirlight->releaseDepthMapBuffer();
    }

    //  PointLights
    for(size_t i = 0; i < scene.numberOfPointLights(); ++i)
    {
        PointLight *current_pointlight = scene.getPointLight(i);
        glViewport(0, 0, current_pointlight->getDepthMapWidth(), current_pointlight->getDepthMapHeight());
        current_pointlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_cubemap_texture_depth_shader.use();
        current_pointlight->sendShadowDatas(m_cubemap_texture_depth_shader);
        scene.drawNoLight(m_cubemap_texture_depth_shader, keys, render_time, current_pointlight->getDepthMapWidth(), current_pointlight->getDepthMapHeight());

        current_pointlight->releaseDepthMapBuffer();
    }

    //  SpotLights
    for(size_t i = 0; i < scene.numberOfSpotLights(); ++i)
    {
        SpotLight *current_spotlight = scene.getSpotLight(i);
        glViewport(0, 0, current_spotlight->getDepthMapWidth(), current_spotlight->getDepthMapHeight());
        current_spotlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_2D_texture_depth_shader.use();
        current_spotlight->sendShadowDatas(m_2D_texture_depth_shader);
        scene.drawNoLight(m_2D_texture_depth_shader, keys, render_time, current_spotlight->getDepthMapWidth(), current_spotlight->getDepthMapHeight());

        current_spotlight->releaseDepthMapBuffer();
    }
}
