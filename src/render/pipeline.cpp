#include "pipeline.h"
#include "scene.h"
#include "renderingquad.h"
#include "a_renderprocess.h"

#include <queue>

Pipeline::Pipeline()
{

}

Pipeline::~Pipeline()
{
    for(std::unordered_map<std::string, ARenderProcess *>::iterator it = m_mapRenderProcesses.begin(); it != m_mapRenderProcesses.end(); ++it)
    {
        delete it->second;
    }
    m_mapRenderProcesses.clear();

    m_apRenderProcessesArray.clear();
}

void Pipeline::reloadShaders()
{
    for(std::unordered_map<std::string, ARenderProcess *>::iterator it = m_mapRenderProcesses.begin(); it != m_mapRenderProcesses.end(); ++it)
    {
        it->second->reloadShaders();
    }
}

Texture *Pipeline::getRenderProcessTexture(const std::string &strRenderProcessKey, const std::string &strTextureKey) const
{
    if(m_mapRenderProcesses.find(strRenderProcessKey) != m_mapRenderProcesses.end())
    {
        return m_mapRenderProcesses[strRenderProcessKey]->getOutTexture(strTextureKey);
    }
    else
    {
        return nullptr;
    }
}

Texture *Pipeline::getOutTexture() const
{
    if(m_sOutput.pRenderProcess != nullptr)
    {
        return m_sOutput.pRenderProcess->getOutTexture(m_sOutput.strTextureKey);
    }
    else
    {
        return nullptr;
    }
}

void Pipeline::preprocessScene(Scene *pScene)
{
    for(size_t i = 0; i < m_apRenderProcessesArray.size(); ++i)
    {
        m_apRenderProcessesArray[i]->preprocessScene(pScene);
    }
}

void Pipeline::processScene(const Scene *pScene, const RenderingQuad *pRenderingQuad, const float &fRenderTime) const
{
    for(size_t i = 0; i < m_apRenderProcessesArray.size(); ++i)
    {
        m_apRenderProcessesArray[i]->processScene(pScene, pRenderingQuad, fRenderTime);
    }
}

bool Pipeline::addRenderProcess(const std::string &strKey, ARenderProcess *pRenderProcess)
{
    if(pRenderProcess != nullptr && strKey.empty() == false && m_mapRenderProcesses.find(strKey) == m_mapRenderProcesses.end())
    {
        m_mapRenderProcesses[strKey] = pRenderProcess;

        return true;
    }
    else
    {
        return false;
    }
}

bool Pipeline::setOutput(const std::string &strRenderProcessKey, const std::string &strTextureKey)
{
    if(m_mapRenderProcesses.find(strRenderProcessKey) != m_mapRenderProcesses.end() && m_mapRenderProcesses[strKey]->hasOutTexture(strTextureKey) == true)
    {
        m_sOutput->pRenderProcess = m_mapRenderProcesses[strKey];
        m_sOutput->strTextureKey = strTextureKey;

        buildRenderProcessesArray();

        return true;
    }
    else
    {
        return false;
    }
}

bool Pipeline::linkRenderProcesses(const std::string &strOutRpKey, const std::string &astrOutTexturesName, const std::string &strInRpKey, const std::string &astrInTexturesName)
{
    //  Check that RenderProcess keys are correct
    if(strInRpKey == strOutRpKey || m_mapRenderProcesses.find(strInRpKey) == m_mapRenderProcesses.end() || m_mapRenderProcesses.find(strOutRpKey) == m_mapRenderProcesses.end())
    {
        return false;
    }

    ARenderProcess *pRpIn = m_mapRenderProcesses[strInRpKey];
    ARenderProcess *pRpOut = m_mapRenderProcesses[strOutRpKey];

    //  Check that there is no loop
    if(checkForLoop(pRpOut, pRpIn) == true)
    {
        return false;
    }

    if(pRpOut->linkProcess(astrOutTexturesNames, pRpIn, astrInTexturesNames) == false)
    {
        //  Link failed
        return false;
    }
buildRenderProcessesArray();
}

bool Pipeline::linkRenderProcesses(const std::string &strOutRpKey, const std::initializer_list<std::string> &astrOutTexturesNames, const std::string &strInRpKey, const std::initializer_list<std::string> &astrInTexturesNames)
{
    //  Check that RenderProcess keys are correct
    if(strInRpKey == strOutRpKey || m_mapRenderProcesses.find(strInRpKey) == m_mapRenderProcesses.end() || m_mapRenderProcesses.find(strOutRpKey) == m_mapRenderProcesses.end())
    {
        return false;
    }

    ARenderProcess *pRpIn = m_mapRenderProcesses[strInRpKey];
    ARenderProcess *pRpOut = m_mapRenderProcesses[strOutRpKey];

    //  Check that there is no loop
    if(checkForLoop(pRpOut, pRpIn) == true)
    {
        return false;
    }

    //  Check that links sizes are correct
    if(astrOutTexturesNames.size() == 0 && astrOutTexturesNames.size() != astrInTexturesNames.size())
    {
        return false;
    }

    for(const std::string *strOutTextureName = astrOutTexturesNames.begin(), *strInTextureName = astrInTexturesNames.begin(); strOutTextureName != astrOutTexturesNames.end(); ++strOutTextureName, ++strInTextureName)
    {
        if(pRpOut->linkProcess(astrOutTexturesNames, pRpIn, astrInTexturesNames) == false)
        {
            //  Link failed
            return false;
        }
    }
    buildRenderProcessesArray();

    return true;
}

/*
 * Returns true if a loop has been detected
 * tries to find pRenderProcessToFind inside
 * InTextures of previous RenderProcesses
 * */
bool Pipeline::checkForLoop(const ARenderProcess *pCurrentRenderProcess, const ARenderProcess *pRenderProcessToFind) const
{
    std::queue<const RenderProcess *> qpProcesses;

    qpProcesses.push(pCurrentRenderProcess);

    //  Check previous RenderProcesses from pCurrentRenderProcess
    while(qpProcesses.empty() == false)
    {
        const ARenderProcess *pCurrentProcess = qpProcesses.front();
        qpProcesses.pop();

        for(size_t i = 0; i < pCurrentProcess->numberOfInputs(); ++i)
        {
            if(pCurrentProcess->getInput(i).pRenderProcess == pRenderProcessToFind)
            {
                //  Loop has been detected
                return false;
            }
            else
            {
                qpProcesses.push(pCurrentProcess->getInput(i).pRenderProcess);
            }
        }
    }

    return false;
}

void Pipeline::setResolution(const GLsizei &iWidth, const GLsizei &iHeight)
{
    for(std::unordered_map<std::string, ARenderProcess *>::iterator it = m_mapRenderProcesses.begin(); it != m_mapRenderProcesses.end(); ++it)
    {
        it->second->setResolution(iWidth, iHeight);
    }
}

/*
 * Build RenderProcesses array
 * using breadth first search
 * in the pipeline
 * */
void Pipeline::buildRenderProcessesArray()
{
    std::queue<ARenderProcess *> qpProcesses;

    qpProcesses.push(m_sOutput.pRenderProcess);

    while(qpProcesses.empty() == false)
    {
        ARenderProcess *pCurrentProcess = qpProcesses.front();
        qpProcesses.pop();

        m_apRenderProcessesArray.push_back(pCurrentProcess);

        for(size_t i = 0; i < pCurrentProcess->numberOfInputs(); ++i)
        {
            qpProcesses.push(pCurrentProcess->getInput(i)->pRenderProcess);
        }
    }

    std::reverse(m_apRenderProcessesArray.begin(), m_apRenderProcessesArray.end());
}
