#include <iostream>

#include "include/render/framebuffer.h"

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

void Framebuffer::attachTextures(const FramebufferTextureDatas *texture_datas, const GLuint &size, GLuint *custom_widths, GLuint *custom_heights)
{
    GLuint *attachments;

    if(size > 1)
        attachments = new GLuint[size];

    m_textures = new Texture[size];

    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

    for(GLuint i = 0; i < size; ++i)
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

        if(size > 1)
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    if(size > 1)
    {
        glDrawBuffers(size, attachments);
        delete[] attachments;

        glGenRenderbuffers(1, &m_render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_render_buffer);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
