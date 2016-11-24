#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H


class Framebuffer
{
public:
    Framebuffer();

    void init(const GLuint &width, const GLuint &height);
    void attachTexture(const GLint &internalFormat, const GLenum &format, const GLenum &type);

    //  Getters
    inline GLuint getBuffer() const{return m_buffer;}

private:
    GLuint m_buffer;
    std::vector<GLuint> m_textures;
    GLuint m_render_buffer;
};

#endif // FRAMEBUFFER_H