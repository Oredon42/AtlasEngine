#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <glew.h>

class RenderBuffer
{
    RenderBuffer(const GLint &iInternalFormat);
    ~RenderBuffer();

    void generate();
    void resize(const GLsizei &iCustomWidth, const GLsizei &iCustomHeight, const bool &bIsSizeFixed = false);

    //  Getters
    inline GLint getInternalFormat() const{return m_iInternalFormat;}
    inline GLuint getId() const{return m_uiId;}
    inline bool isSizeFixed() const{return m_bIsSizeFixed;}

private:
    GLuint m_uiId;
    GLint m_iInternalFormat;

    GLsizei m_iWidth;
    GLsizei m_iHeight;

    bool m_bIsSizeFixed;
};

#endif // RENDERBUFFER_H
