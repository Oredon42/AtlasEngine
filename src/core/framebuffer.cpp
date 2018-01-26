#include <iostream>

#include "core/data/framebuffer.h"
#include "core/data/renderbuffer.h"
#include "core/data/texture.h"

Framebuffer::Framebuffer() :
    m_uiBuffer(0)
{

}

Framebuffer::~Framebuffer()
{
    for(size_t i = 0; i < m_aTextures.size(); ++i)
    {
        delete m_aTextures[i];
    }
    m_aTextures.clear();

    for(size_t i = 0; i < m_aRenderBuffers.size(); ++i)
    {
        delete m_aRenderBuffers[i];
    }
    m_aRenderBuffers.clear();
}

void Framebuffer::init(const GLsizei &iWidth, const GLsizei &iHeight)
{
    glGenFramebuffers(1, &m_uiBuffer);
    m_iWidth = iWidth;
    m_iHeight = iHeight;
}

void Framebuffer::resize(const GLsizei &iWidth, const GLsizei &iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    for(size_t i = 0; i < m_aTextures.size(); ++i)
    {
        if(m_aTextures[i]->isSizeFixed() == GL_FALSE)
        {
            m_aTextures.resize(m_iWidth, m_iHeight);
        }
    }

    for(size_t i = 0; i < m_aRenderBuffers.size(); ++i)
    {
        if(m_aRenderBuffers[i]->isSizeFixed() == GL_FALSE)
        {
            m_aRenderBuffers.resize(m_iWidth, m_iHeight);
        }
    }
}

void Framebuffer::attachTextures(const std::vector<TextureAttachment> &aTexturesAttachments)
{
    if(aTexturesAttachments.size() > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_uiBuffer);

        //  Get maximum color attachments
        GLint iCurrentMaxColorAttachments = -1;
        GLint iMaxColorAttachments = 0;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);

        for(size_t i = 0; i < aTexturesAttachments.size(); ++i)
        {
            if(aTexturesAttachments[i].pTexture != 0)
            {
                aTexturesAttachments[i].pTexture->resize(m_iWidth, m_iHeight);
                m_aTextures.push_back(aTexturesAttachments[i].pTexture);

                //  If Texture attachment is color, store maximum
                if(aTexturesAttachments[i].eAttachment >= GL_COLOR_ATTACHMENT0 && aTexturesAttachments[i].eAttachment < (GL_COLOR_ATTACHMENT0 + iMaxColorAttachments))
                {
                    iCurrentMaxColorAttachments = std::max(iCurrentMaxColorAttachments, (GLint)(aTexturesAttachments[i].eAttachment - GL_COLOR_ATTACHMENT0));
                }

                //  Attach texture
                switch(aTexturesAttachments[i].pTexture->getTarget())
                {
                case GL_TEXTURE_2D:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, aTexturesAttachments[i].eAttachment, GL_TEXTURE_2D, m_aTextures[i]->getId(), 0);
                    break;

                default:
                    glFramebufferTexture(GL_FRAMEBUFFER, aTexturesAttachments[i].eAttachment, m_aTextures[i]->getId(), 0);
                    break;
                }
            }
        }

        //  If color attachments are used, inform OpenGL
        if(iCurrentMaxColorAttachments != -1)
        {
            GLenum *aeColorAttachments = new GLenum[iCurrentMaxColorAttachments + 1];

            for(GLint i = 0; i <= iCurrentMaxColorAttachments; ++i)
            {
                aeColorAttachments[iCurrentMaxColorAttachments] = GL_COLOR_ATTACHMENT0 + (GLenum)i;
            }

            glDrawBuffers(m_aTextures.size(), aeColorAttachments);
            delete[] aeColorAttachments;
        }

        //  Check that Framebuffer is complete
        const GLenum &eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(eStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer::attachTextures::Framebuffer not complete: " << eStatus << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Framebuffer::attachRenderBuffers(const std::vector<RenderbufferAttachment> &aRenderbuffersAttachments)
{
    if(aRenderbuffersAttachments.size() > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_uiBuffer);

        for(size_t i = 0; i < aRenderbuffersAttachments.size(); ++i)
        {
            if(aRenderbuffersAttachments[i].pRenderBuffer != 0)
            {
                aRenderbuffersAttachments[i].pRenderBuffer->resize(m_iWidth, m_iHeight);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, aRenderbuffersAttachments[i].eAttachment, GL_RENDERBUFFER, aRenderbuffersAttachments[i].pRenderBuffer->getId());
            }
        }

        //  Check that Framebuffer is complete
        const GLenum &eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(eStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer::attachTextures::Framebuffer not complete: " << eStatus << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
