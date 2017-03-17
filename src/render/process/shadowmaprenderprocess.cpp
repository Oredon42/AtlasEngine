#include "include/render/process/shadowmaprenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

ShadowMapRenderProcess::ShadowMapRenderProcess(const GLuint nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights) :
    RenderProcess(1)
{

}

void ShadowMapRenderProcess::init(const GLuint &width, const GLuint &height)
{
    //  DirLights + SpotLights
    m_dirlights_depth_shader.init("shaders/2D_texture_depth.vert", "shaders/2D_texture_depth.frag");

    //  PointLights
    m_pointlights_depth_shader.init("shaders/cubemap_depth.vert", "shaders/cubemap_depth.frag", "shaders/cubemap_depth.geom");
}

void ShadowMapRenderProcess::initMenuElement()
{

}

void ShadowMapRenderProcess::resize(const GLuint &width, const GLuint &height)
{

}

void ShadowMapRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    //  DirLights
    for(size_t i = 0; i < scene.numberOfDirLights(); ++i)
    {
        DirLight *current_dirlight = scene.getDirLight(i);
        glViewport(0, 0, current_dirlight->getDepthMapWidth(), current_dirlight->getDepthMapHeight());
        current_dirlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_dirlights_depth_shader.use();
        current_dirlight->sendShadowDatas(m_dirlights_depth_shader);
        scene.drawNoLight(m_dirlights_depth_shader, keys, render_time, current_dirlight->getDepthMapWidth(), current_dirlight->getDepthMapHeight());

        current_dirlight->releaseDepthMapBuffer();
    }

    //  PointLights
    for(size_t i = 0; i < scene.numberOfPointLights(); ++i)
    {
        PointLight *current_pointlight = scene.getPointLight(i);
        glViewport(0, 0, current_pointlight->getDepthMapWidth(), current_pointlight->getDepthMapHeight());
        current_pointlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_pointlights_depth_shader.use();
        current_pointlight->sendShadowDatas(m_pointlights_depth_shader);
        scene.drawNoLight(m_pointlights_depth_shader, keys, render_time, current_pointlight->getDepthMapWidth(), current_pointlight->getDepthMapHeight());

        current_pointlight->releaseDepthMapBuffer();
    }

    //  SpotLights
    for(size_t i = 0; i < scene.numberOfSpotLights(); ++i)
    {
        SpotLight *current_spotlight = scene.getSpotLight(i);
        glViewport(0, 0, current_spotlight->getDepthMapWidth(), current_spotlight->getDepthMapHeight());
        current_spotlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_dirlights_depth_shader.use();
        current_spotlight->sendShadowDatas(m_dirlights_depth_shader);
        scene.drawNoLight(m_dirlights_depth_shader, keys, render_time, current_spotlight->getDepthMapWidth(), current_spotlight->getDepthMapHeight());

        current_spotlight->releaseDepthMapBuffer();
    }
}
