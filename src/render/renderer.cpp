#include "include/render/renderer.h"
#include "include/data/scene.h"
#include "include/data/lighting/pointlight.h"

#include <QOpenGLFramebufferObject>

Renderer::Renderer() :
    m_quad(1.f)
{

}

void Renderer::init(const std::string &path, const GLuint &width, const GLuint &height, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights)
{
    setDimensions(width, height);
    m_quad.setupBuffers();

    m_pipelines.push_back(Pipeline(width, height));
    m_pipelines[0].addProcess(new GeometryRenderProcess(width, height));
    m_pipelines[0].addProcess(new SSAORenderProcess(width, height));
    m_pipelines[0].addProcess(new LightingRenderProcess(width, height, nb_dirlights, nb_pointlights, nb_spotlights));
    m_pipelines[0].addProcess(new HDRRenderProcess(width, height));

    m_quad_shader.init("shaders/quad.vert", "shaders/quad.frag");
}

void Renderer::resize(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;
}

/*
 * Render a scene using deferred rendering
 * */
void Renderer::drawScene(const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const
{
    m_pipelines[0].process(m_quad, scene, render_time, keys);

    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT);
    m_quad_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pipelines[0].getOutTexture());
    m_quad.draw();
}

void Renderer::reloadShaders()
{
    //for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    //    m_shader_geometry_pass[i].reload();
}
