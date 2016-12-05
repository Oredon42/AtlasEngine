#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "openglincludes.h"
#include "texture.h"

struct FramebufferTextureDatas
{
    GLint internal_format;
    GLenum format;
    GLenum type;

    FramebufferTextureDatas(const GLint &i_f, const GLenum &f, const GLenum &t) :
        internal_format(i_f),
        format(f),
        type(t)
    {
    }
};

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void init(const GLuint &width, const GLuint &height);
    void attachTextures(const FramebufferTextureDatas* texture_datas, const GLuint &size, GLint clamp = GL_FALSE, GLuint renderbuffer = GL_FALSE);

    //  Getters
    inline GLuint getBuffer() const{return m_buffer;}
    inline GLuint getTexture(const GLuint &i) const{return m_textures[i].getId();}

private:
    GLuint m_buffer;
    Texture *m_textures;
    GLuint m_render_buffer;

    GLuint m_width;
    GLuint m_height;
};

#endif // FRAMEBUFFER_H
