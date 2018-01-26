#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include <unordered_map>

#include <glew.h>

class Pipeline;
class RenderingQuad;
class Scene;
class Shader;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();

    void process(const float &fRenderTime) const;

    void reloadShaders();

    void addPipeline(const std::string &strKey, Pipeline *pPipeline);

    //  Setters
    void setCurrentScene(Scene *pScene);
    void setCurrentPipeline(const std::string &strKey);
    void setResolution(const GLsizei &iWidth, const GLsizei &iHeight);

private:
    GLsizei m_iWidth;
    GLsizei m_iHeight;

    RenderingQuad *m_pRenderingQuad;
    Shader *m_pRenderingQuadShader;

    std::unordered_map<std::string, Pipeline *> m_pPipelinesMap;
    Pipeline *m_pCurrentPipeline;

    const Scene *m_pCurrentScene;
};

#endif // RENDERER_H
