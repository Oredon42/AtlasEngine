#include "core/data/texture.h"

Texture::Texture(const GLint &iInternalFormat, const GLenum &eFormat, const GLenum &eType, GLint iClamp, GLint iFilterMin, GLint iFilterMax, const GLfloat (&afBorderColor)[4], GLboolean bIsDepth, GLenum eTarget) :
    m_uiId(0),
    m_iInternalFormat(iInternalFormat),
    m_eFormat(eFormat),
    m_eType(eType),
    m_iClamp(iClamp),
    m_iFilterMin(iFilterMin),
    m_iFilterMax(iFilterMax),
    m_bIsDepth(bIsDepth),
    m_eTarget(eTarget),
    m_pData(NULL),
    m_iWidth(0),
    m_iHeight(0),
    m_bIsSizeFixed(false),
    m_strFilePath("")
{
    m_afBorderColor[0] = afBorderColor[0];
    m_afBorderColor[1] = afBorderColor[1];
    m_afBorderColor[2] = afBorderColor[2];
    m_afBorderColor[3] = afBorderColor[3];
}

Texture::Texture(const std::string &strFilePath) :
    m_uiId(0),
    m_iInternalFormat(GL_RGBA),
    m_eFormat(GL_RGBA),
    m_eType(GL_UNSIGNED_BYTE),
    m_iClamp(GL_REPEAT),
    m_iFilterMin(GL_LINEAR_MIPMAP_LINEAR),
    m_iFilterMax(GL_NEAREST),
    m_bIsDepth(GL_FALSE),
    m_eTarget(GL_TEXTURE_2D),
    m_pData(NULL),
    m_iWidth(0),
    m_iHeight(0),
    m_bIsSizeFixed(false),
    m_strFilePath(strFilePath)
{

}

Texture::~Texture()
{
    if(m_uiId != 0)
    {
        glDeleteTextures(1, &m_uiId);
    }
}

void Texture::generate()
{
    //  If has file path, load Texture from it
    if(m_strFilePath != "")
    {
        loadTextureFile();
    }

    //  If id not generated yet, generate it
    if(m_uiId == 0)
    {
        glGenTextures(1, &m_uiId);
    }
    glBindTexture(m_eTarget, m_uiId);

    //  glTexImage2D

    if(m_eTarget == GL_TEXTURE_CUBE_MAP)
    {
        for(GLuint i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_iInternalFormat, m_iWidth, m_iHeight, 0, m_eFormat, m_eType, m_pData);
        }
    }
    else
    {
        glTexImage2D(m_eTarget, 0, m_iInternalFormat, m_iWidth, m_iHeight, 0, m_eFormat, m_eType, m_pData);
    }

    //  glTexParameters

    glTexParameteri(m_eTarget, GL_TEXTURE_MIN_FILTER, m_iFilterMin);
    glTexParameteri(m_eTarget, GL_TEXTURE_MAG_FILTER, m_iFilterMax);

    if(m_iClamp != GL_NONE)
    {
        glTexParameteri(m_eTarget, GL_TEXTURE_WRAP_S, m_iClamp);
        glTexParameteri(m_eTarget, GL_TEXTURE_WRAP_T, m_iClamp);

        if(m_eTarget == GL_TEXTURE_CUBE_MAP)
        {
            glTexParameteri(m_eTarget, GL_TEXTURE_WRAP_R, m_iClamp);
        }
    }
    glTexParameterfv(m_eTarget, GL_TEXTURE_BORDER_COLOR, m_afBorderColor);

    if(m_iFilterMin != GL_NEAREST && m_iFilterMin != GL_LINEAR)
    {
        glGenerateMipmap(m_eTarget);
    }

    glBindTexture(m_eTarget, 0);
}

void Texture::resize(const GLsizei &iCustomWidth, const GLsizei &iCustomHeight)
{
    if(m_bIsSizeFixed == false)
    {
        m_iWidth = iCustomWidth;
        m_iHeight = iCustomHeight;

        generate();
    }
}

void Texture::loadTextureFile()
{
    //  TODO: load with sfml
    const QImage imageContainer(m_strFilePath.c_str());

    switch(imageContainer.format())
    {
    case QImage::Format_RGB32:
        m_eFormat = GL_RGBA;
        break;

    case QImage::Format_RGB16:
        m_eFormat = GL_RGBA;
        break;

    case QImage::Format_Grayscale8:
        m_iInternalFormat = GL_R16F;
        m_eFormat = GL_RED;
        break;

    default:
        break;
    }

    //  Get scaled (if needed) image data
    if(m_iWidth != 0 && m_iHeight != 0 && (m_iWidth != imageContainer.width() || m_iHeight != imageContainer.height()))
    {
        const QImage scaledImage = imageContainer.scaled(m_iWidth, m_iHeight);
        m_pData = scaledImage.bits();
    }
    else
    {
        m_pData = imageContainer.bits();
    }
}
