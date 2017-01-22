#include "include/render/renderer.h"
#include "include/data/scene.h"
#include "include/data/lighting/pointlight.h"

#include <QOpenGLFramebufferObject>

Renderer::Renderer() :
    m_quad(1.f),
    m_current_pipeline(0)
{

}

Renderer::~Renderer()
{
    for(GLuint i = 0; i < m_pipelines.size(); ++i)
        delete m_pipelines[i];

    m_pipelines.clear();
}

void Renderer::init(const GLuint &width, const GLuint &height)
{
    setDimensions(width, height);

    m_quad.setupBuffers();
    m_quad_shader.init("shaders/quad.vert", "shaders/quad.frag");
}

void Renderer::addPipeline(Pipeline *pipeline, const std::string &pipeline_name)
{
    m_pipelines.push_back(pipeline);
    if(m_pipelines_map.find(pipeline_name) == m_pipelines_map.end())
        m_pipelines_map[pipeline_name] = pipeline;
}

/*
 * Render a scene using deferred rendering
 * */
void Renderer::drawScene(const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const
{
    m_current_pipeline->process(m_quad, scene, render_time, keys);

    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT);
    m_quad_shader.use();
    glActiveTexture(GL_TEXTURE0);
    m_current_pipeline->getOutTexture()->bind();
    m_quad.draw();
}

void Renderer::reloadShaders()
{
    //for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    //    m_shader_geometry_pass[i].reload();
}

void Renderer::setCurrentPipeline(std::string pipeline_name)
{
    if(m_pipelines_map.find(pipeline_name) != m_pipelines_map.end())
        m_current_pipeline = m_pipelines_map[pipeline_name];
}
