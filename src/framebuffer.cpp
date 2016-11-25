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

void Framebuffer::attachTextures(const FramebufferTextureDatas *texture_datas, const GLuint &size)
{
    GLuint *attachments = new GLuint[size];
    m_textures = new GLuint[size];

    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

    for(GLuint i = 0; i < size; ++i)
    {
        glGenTextures(1, &m_textures[i]);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, texture_datas[i].internal_format, m_width, m_height, 0, texture_datas[i].format, texture_datas[i].type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);

        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glDrawBuffers(size, attachments);
    delete[] attachments;

    glGenRenderbuffers(1, &m_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_render_buffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
