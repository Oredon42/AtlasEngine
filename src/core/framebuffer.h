#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>

#include <glew.h>
#include <glm/glm.hpp>

class Texture;
class RenderBuffer;

struct TextureAttachment
{
    Texture *pTexture;
    GLenum eAttachment;
};

struct RenderbufferAttachment
{
    RenderBuffer *pRenderBuffer;
    GLenum eAttachment;
};

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void init(const GLsizei &iWidth, const GLsizei &iHeight);
    void resize(const GLsizei &iWidth, const GLsizei &iHeight);

    void attachTextures(const std::vector<TextureAttachment> &aTexturesAttachments);
    void attachRenderBuffers(const std::vector<RenderbufferAttachment> &aRenderbuffersAttachments);

    inline void bind(GLenum eTarget = GL_FRAMEBUFFER) const{glBindFramebuffer(eTarget, m_uiBuffer);}
    inline void release(GLenum eTarget = GL_FRAMEBUFFER) const{glBindFramebuffer(eTarget, 0);}

    //  Getters
    inline GLsizei getWidth() const{return m_iWidth;}
    inline GLsizei getHeight() const{return m_iHeight;}
    inline GLuint getBuffer() const{return m_uiBuffer;}
    inline Texture *getTexture(const GLuint &i) const{return m_aTextures[i];}

private:
    GLuint m_uiBuffer;

    std::vector<Texture *> m_aTextures;
    std::vector<RenderBuffer *> m_aRenderBuffers;

    GLsizei m_iWidth;
    GLsizei m_iHeight;
};

#endif // FRAMEBUFFER_H
