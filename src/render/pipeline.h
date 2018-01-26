#ifndef PIPELINE_H
#define PIPELINE_H

#include <openglincludes.h>
#include <vector>
#include <unordered_map>

class RenderingQuad;
class ARenderProcess;
class Scene;
class Texture;

class Pipeline
{
    struct Output
    {
        ARenderProcess *pRenderProcess = nullptr;
        std::string strTextureKey;
    };

public:
    Pipeline();
    ~Pipeline();

    void preprocess(Scene *pScene);
    void process(const RenderingQuad *pRenderingQuad, const float &fRenderTime) const;

    void reloadShaders();

    //  Getter
    Texture *getRenderProcessTexture(const std::string &strRenderProcessKey, const std::string &strTextureKey) const;
    Texture *getOutTexture() const;

    //  Setter
    bool addRenderProcess(const std::string &strKey, ARenderProcess *pRenderProcess);
    bool setOutput(const std::string &strRenderProcessKey, const std::string &strTextureKey);
    bool linkRenderProcesses(const std::string &strOutRpKey, const std::string &astrOutTexturesName, const std::string &strInRpKey, const std::string &astrInTexturesName);
    bool linkRenderProcesses(const std::string &strOutRpKey, const std::initializer_list<std::string> &astrOutTexturesNames, const std::string &strInRpKey, const std::initializer_list<std::string> &astrInTexturesNames);
    void setResolution(const GLsizei &iWidth, const GLsizei &iHeight);

private:
    std::unordered_map<std::string, ARenderProcess *> m_mapRenderProcesses;
    std::vector<ARenderProcess *> m_apRenderProcessesArray;

    Output m_sOutput;

    //  Private methods
    bool checkForLoop(const ARenderProcess *pCurrentRenderProcess, const ARenderProcess *pRenderProcessToFind) const;
    void buildRenderProcessesArray();
};

#endif // PIPELINE_H
