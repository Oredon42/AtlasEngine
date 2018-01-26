#ifndef A_RENDERPROCESS_H
#define A_RENDERPROCESS_H

#include <glew.h>

#include <initializer_list>
#include <unordered_map>

class Quad;
class Framebuffer;
class Scene;
class Texture;
class Pipeline;
class RenderingQuad;

class ARenderProcess
{
    friend class Pipeline;

    struct Input
    {
        GLint iInternalFormat;
        std::string strName;
        ARenderProcess *pRenderProcess = nullptr;
    };

public:
    virtual ~ARenderProcess();

    virtual void init(const GLsizei &iWidth, const GLsizei &iHeight);
    inline virtual void resize(const GLsizei &iWidth, const GLsizei &iHeight) {m_iWidth = iWidth; m_iHeight = iHeight;}

    virtual void preprocess(Scene *pScene) = 0;
    virtual void process(const RenderingQuad *pRenderingQuad, const float &fRenderTime) = 0;

    virtual void reloadShaders() = 0;

    //  Getters
    Input *getInput(const std::string &strKey) const;
    Texture *getOutTexture(const std::string &strKey) const;
    inline GLsizei getWidth() const{return m_iWidth;}
    inline GLsizei getHeight() const{return m_iHeight;}

    inline size_t numberOfInputs() const{return m_mapInputs.size();}
    inline size_t numberOfOutputs() const{return m_mapOutTextures.size();}

    //  Setters
    inline virtual void setActivated(const bool &bActivated){m_bActivated = bActivated;}

protected:
    virtual void allocate() {}
    bool linkProcess(const std::initializer_list<std::string> &astrOutTexturesNames, ARenderProcess *pInRenderProcess, const std::initializer_list<std::string> &astrInTexturesNames);
    bool bindInTexture(const std::string &strKey) const{m_mapInputs[strKey]->pRenderProcess->getOutTexture(m_mapInputs[strKey]->strName)->bind();}

private:
    GLsizei m_iWidth;
    GLsizei m_iHeight;

    bool m_bActivated;
    bool m_bInitialized = false;

    std::unordered_map<std::string, Input *> m_mapInputs;
    std::unordered_map<std::string, Texture *> m_mapOutTextures;
};

#endif // A_RENDERPROCESS_H
