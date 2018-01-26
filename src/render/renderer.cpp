#include "renderer.h"

#include "scene.h"
#include "shader.h"
#include "pipeline.h"
#include "renderingquad.h"

#include <QOpenGLFramebufferObject>

Renderer::Renderer() :
    m_iWidth(256),
    m_iHeight(256),
    m_pRenderingQuad(nullptr),
    m_pRenderingQuadShader(nullptr),
    m_pCurrentPipeline(nullptr),
    m_pCurrentScene(nullptr)
{

}

Renderer::~Renderer()
{
    for(std::unordered_map<std::string, Pipeline *>::iterator it = m_pPipelinesMap.begin(); it != m_pPipelinesMap.end(); ++it)
    {
        delete it->second;
    }
    m_pPipelinesMap.clear();

    if(m_pRenderingQuad != nullptr)
    {
        delete m_pRenderingQuad;
    }

    if(m_pRenderingQuadShader != nullptr)
    {
        delete m_pRenderingQuadShader;
    }
}

void Renderer::init()
{
    if(m_pRenderingQuad == nullptr)
    {
        m_pRenderingQuad = new RenderingQuad(1.f);
    }

    if(m_pRenderingQuadShader == nullptr)
    {
        m_pRenderingQuadShader = new Shader("shaders/quad.vert", "shaders/quad.frag");
    }
    m_pRenderingQuadShader->resetActiveTexture();
}

void Renderer::process(const float &fRenderTime) const
{
    if(m_pCurrentPipeline != nullptr)
    {
        //  Process pipeline
        m_pCurrentPipeline->process(m_pRenderingQuad, fRenderTime);

        //  Render final output
        glViewport(0, 0, m_iWidth, m_iHeight);
        QOpenGLFramebufferObject::bindDefault();
        glClear(GL_COLOR_BUFFER_BIT);
        m_pRenderingQuadShader->use();
        glActiveTexture(m_pRenderingQuadShader->getActiveTexture());
        m_pCurrentPipeline->getLastTexture()->bind();
        m_pRenderingQuad->draw();
    }
}

void Renderer::reloadShaders()
{
    m_pRenderingQuadShader->reload();
    m_pCurrentPipeline->reloadShaders();
}

void Renderer::addPipeline(const std::string &strKey, Pipeline *pPipeline)
{
    if(m_pPipelinesMap.find(strKey) == m_pPipelinesMap.end())
    {
        m_pPipelinesMap[strKey] = pPipeline;
        m_pPipelinesMap[strKey]->setResolution(m_iWidth, m_iHeight);
    }
}

void Renderer::setCurrentScene(Scene *pScene)
{
    m_pCurrentScene = pScene;

    if(m_pCurrentPipeline != nullptr)
    {
        m_pCurrentPipeline->preprocessScene(m_pCurrentScene);
    }
}

void Renderer::setCurrentPipeline(const std::string &strKey)
{
    if(m_pPipelinesMap.find(strKey) != m_pPipelinesMap.end())
    {
        m_pCurrentPipeline = m_pPipelinesMap[strKey];

        if(m_pCurrentScene != nullptr)
        {
            m_pCurrentPipeline->preprocessScene(m_pCurrentScene);
        }
    }
}

void Renderer::setResolution(const GLsizei &iWidth, const GLsizei &iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    for(std::unordered_map<std::string, Pipeline *>::iterator it = m_pPipelinesMap.begin(); it != m_pPipelinesMap.end(); ++it)
    {
        it->second->setResolution(iWidth, iHeight);
    }
}
