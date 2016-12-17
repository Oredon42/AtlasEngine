#include <iostream>

#include "include/render/framebuffer.h"

Framebuffer::Framebuffer() :
    m_textures(0),
    m_renderbuffers(0)
{

}

Framebuffer::~Framebuffer()
{
    if(m_textures != 0)
        delete[] m_textures;

    if(m_renderbuffers != 0)
        delete[] m_renderbuffers;
}

void Framebuffer::init(const GLuint &width, const GLuint &height)
{
    glGenFramebuffers(1, &m_buffer);
    m_width = width;
    m_height = height;
}

void Framebuffer::attachTextures(const FramebufferTextureDatas* texture_datas, const GLuint &texture_size, GLuint *custom_widths, GLuint *custom_heights)
{
    if(texture_size > 0)
    {
        GLuint *attachments;

        if(texture_size > 1)
            attachments = new GLuint[texture_size];

        m_textures = new Texture[texture_size];

        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

        for(GLuint i = 0; i < texture_size; ++i)
        {
            GLuint width, height;
            if(custom_widths == 0 || custom_heights == 0)
            {
                width = m_width;
                height = m_height;
            }
            else
            {
                width = custom_widths[i];
                height = custom_heights[i];
            }
            m_textures[i].init(texture_datas[i].internal_format, width, height, texture_datas[i].format, texture_datas[i].type, NULL, texture_datas->clamp, texture_datas->filter_max, texture_datas->filter_min);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i].getId(), 0);

            if(texture_size > 1)
                attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        if(texture_size > 1)
        {
            glDrawBuffers(texture_size, attachments);
            delete[] attachments;
        }

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer::attachTextures::Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Framebuffer::attachRenderBuffers(FramebufferRenderbufferDatas *renderbuffer_datas, GLuint renderbuffer_size, GLuint *custom_widths, GLuint *custom_heights)
{
    if(renderbuffer_size > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

        m_renderbuffers = new GLuint[renderbuffer_size];
        glGenRenderbuffers(renderbuffer_size, m_renderbuffers);

        for(GLuint i = 0; i < renderbuffer_size; ++i)
        {
            GLuint width, height;
            if(custom_widths == 0 || custom_heights == 0)
            {
                width = m_width;
                height = m_height;
            }
            else
            {
                width = custom_widths[i];
                height = custom_heights[i];
            }

            glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffers[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, renderbuffer_datas->internal_format, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbuffer_datas->attachment, GL_RENDERBUFFER, m_renderbuffers[i]);
        }

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer::attachRenderBuffers::Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
