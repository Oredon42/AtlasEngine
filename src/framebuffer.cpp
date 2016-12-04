#include <iostream>

#include "framebuffer.h"

Framebuffer::Framebuffer() :
    m_textures(0)
{

}

Framebuffer::~Framebuffer()
{
    if(m_textures != 0)
        delete[] m_textures;
}

void Framebuffer::init(const GLuint &width, const GLuint &height)
{
    glGenFramebuffers(1, &m_buffer);
    m_width = width;
    m_height = height;
}

void Framebuffer::attachTextures(const FramebufferTextureDatas *texture_datas, const GLuint &size, GLint clamp, GLuint renderbuffer)
{
    GLuint *attachments;

    if(size > 0)
        attachments = new GLuint[size];

    m_textures = new Texture[size];

    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

    for(GLuint i = 0; i < size; ++i)
    {
        m_textures[i].init(texture_datas[i].internal_format, m_width, m_height, texture_datas[i].format, texture_datas[i].type, NULL, clamp, GL_NEAREST, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i].getId(), 0);

        if(size > 0)
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    if(size > 0)
    {
        glDrawBuffers(size, attachments);
        delete[] attachments;
    }

    if(renderbuffer)
    {
        glGenRenderbuffers(1, &m_render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_render_buffer);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
