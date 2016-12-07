#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "openglincludes.h"
#include "texture.h"

struct FramebufferTextureDatas
{
    GLint internal_format;
    GLenum format;
    GLenum type;
    GLint clamp;
    GLuint filter_min;
    GLuint filter_max;

    FramebufferTextureDatas(const GLint &i_f, const GLenum &f, const GLenum &t, GLint c = GL_FALSE, GLuint f_min = GL_NEAREST, GLuint f_max = GL_NEAREST) :
        internal_format(i_f),
        format(f),
        type(t),
        clamp(c),
        filter_min(f_min),
        filter_max(f_max)
    {
    }
};

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void init(const GLuint &width, const GLuint &height);
    void attachTextures(const FramebufferTextureDatas* texture_datas, const GLuint &size, GLuint *custom_widths = 0, GLuint *custom_heights = 0);

    inline void bind(GLenum target = GL_FRAMEBUFFER)const {glBindFramebuffer(target, m_buffer);}

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
