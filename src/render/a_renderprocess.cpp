#include "a_renderprocess.h"

#include "texture.h"

ARenderProcess::~ARenderProcess()
{
    for(std::unordered_map<std::string, Texture *>::iterator it = m_mapOutTextures.begin(); it != m_mapOutTextures.end(); ++it)
    {
        delete it->second;
    }
}

void ARenderProcess::init(const GLsizei &iWidth, const GLsizei &iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    allocate();

    m_bInitialized = true;
}

ARenderProcess::Input *ARenderProcess::getInput(const std::string &strKey) const
{
    if(m_mapInputs.find(strKey) != m_mapInputs.end())
    {
        return m_mapInputs[strKey];
    }
    else
    {
        return nullptr;
    }
}

Texture *ARenderProcess::getOutTexture(const std::string &strKey) const
{
    if(m_mapOutTextures.find(strKey) != m_mapOutTextures.end())
    {
        return m_mapOutTextures[strKey];
    }
    else
    {
        return nullptr;
    }
}

/*
 * Connect this process out Textures to another process in Textures
 * if some Textures are already linked, old link is deleted
 * */
bool ARenderProcess::linkProcess(const std::initializer_list<std::string> &astrOutTexturesNames, ARenderProcess *pInRenderProcess, const std::initializer_list<std::string> &astrInTexturesNames)
{
    //  Check that links are the same size
    if(astrOutTexturesNames.size() > 0 && astrOutTexturesNames.size() == astrInTexturesNames.size())
    {
        //  For each Texture to link
        for(const std::string *strOutTextureName = astrOutTexturesNames.begin(), *strInTextureName = astrInTexturesNames.begin(); strOutTextureName != astrOutTexturesNames.end(); ++strOutTextureName, ++strInTextureName)
        {
            //  Check that names are correct and internal formats are the same
            if(m_mapOutTextures.find(strOutTextureName) != m_mapOutTextures.end() && pInRenderProcess->m_mapInputs.find(strInTextureName) != pInRenderProcess->m_mapInputs.end()
                    && m_mapOutTextures[strOutTextureName]->getInternalFormat() == pInRenderProcess->m_mapInputs[pInRenderProcess]->iInternalFormat)
            {
                //  Check that it's not already linked to the RenderProcess
                if(pInRenderProcess->m_mapInputs[pInRenderProcess]->pRenderProcess != this)
                {
                    //  Create new link
                    pInRenderProcess->m_mapInputs[pInRenderProcess]->pRenderProcess = this;
                    pInRenderProcess->m_mapInputs[pInRenderProcess]->strName = strOutTextureName;
                }
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

void ARenderProcess::bindInTexture(const std::string &strKey) const
{
    if(m_mapInputs.find(strKey) != m_mapInputs.end())
    {
        m_mapInputs[strKey]->pRenderProcess->getOutTexture(m_mapInputs[strKey]->strName)->bind();

        return true;
    }
    else
    {
        return false;
    }
}
