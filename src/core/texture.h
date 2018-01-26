#ifndef TEXTURE_H
#define TEXTURE_H

#include <glew.h>
#include <glm/glm.hpp>
#include <string>

constexpr GLfloat DEFAULT_BORDER_COLOR[4] = {0.f, 0.f, 0.f, 0.f};

class Texture
{
public:
    Texture(const GLint &iInternalFormat = GL_RGBA,
            const GLenum &eFormat = GL_RGBA,
            const GLenum &eType = GL_UNSIGNED_BYTE,
            const GLint &iClamp = GL_FALSE,
            const GLint &iFilterMin = GL_NEAREST,
            const GLint &iFilterMax = GL_NEAREST,
            const GLfloat (&afBorderColor)[4] = DEFAULT_BORDER_COLOR,
            const GLboolean &bIsDepth = GL_FALSE,
            const GLenum &eTarget = GL_TEXTURE_2D);
    Texture(const std::string &strFilePath);
    ~Texture();

    inline void bind() const{glBindTexture(m_eTarget, m_uiId);}
    void generate();
    void resize(const GLsizei &iCustomWidth, const GLsizei &iCustomHeight);

    //  Setters
    void setFilePath(const std::string &strFilePath) {m_strFilePath = strFilePath;}

    //  Getters
    inline GLuint getId() const{return m_uiId;}
    inline GLenum getTarget() const{return m_eTarget;}
    inline GLint getInternalFormat() const{return m_iInternalFormat;}
    inline std::string getPath() const{return m_strFilePath;}
    inline bool isSizeFixed() const{return m_bIsSizeFixed;}

private:
    GLuint m_uiId;

    GLint m_iInternalFormat;    //  GL_R8, GL_RGB16F ...
    GLenum m_eFormat;           //  GL_RED, GL_RGB ...
    GLenum m_eType;             //  GL_UNSIGNED_BYTE, GL_FLOAT ...
    GLint m_iClamp;             //  GL_CLAMP_TO_BORDER ... (or GL_FALSE)
    GLint m_iFilterMin;         //  GL_NEAREST ...
    GLint m_iFilterMax;         //  GL_NEAREST ...
    GLfloat m_afBorderColor[4]; //  Default = {0, 0, 0, 0}
    GLboolean m_bIsDepth;       //  Depth map?
    GLenum m_eTarget;           //  GL_TEXTURE_2D ...
    GLvoid *m_pData;            //  != NULL if image
    GLsizei m_iWidth;
    GLsizei m_iHeight;

    bool m_bIsSizeFixed;

    std::string m_strFilePath;

    //  Private methods
    void loadTextureFile();
};

#endif // TEXTURE_H
