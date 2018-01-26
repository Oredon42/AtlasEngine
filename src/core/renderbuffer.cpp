#include "core/data/renderbuffer.h"

RenderBuffer::RenderBuffer(const GLint &iInternalFormat) :
    m_uiId(0),
    m_iInternalFormat(iInternalFormat)
{

}

RenderBuffer::~RenderBuffer()
{
    glDeleteRenderbuffers(1, &m_uiId);
}

void RenderBuffer::generate()
{
    if(m_uiId == 0)
    {
        glGenRenderbuffers(1, &m_uiId);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, m_uiId);
    glRenderbufferStorage(GL_RENDERBUFFER, m_iInternalFormat, m_iWidth, m_iHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::resize(const GLsizei &iCustomWidth, const GLsizei &iCustomHeight, const bool &bIsSizeFixed)
{
    if(m_bIsSizeFixed == false)
    {
        m_iWidth = iCustomWidth;
        m_iHeight = iCustomHeight;
        m_bIsSizeFixed = bIsSizeFixed;

        generate();
    }
}
