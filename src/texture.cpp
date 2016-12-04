#include "texture.h"

Texture::Texture()
{

}

Texture::Texture(const GLint &internal_format, const GLsizei &width, const GLsizei &height, const GLenum &format, const GLenum &type, const GLvoid *data, GLuint clamp, GLuint filter_min, GLuint filter_max, std::string path, std::string shading_type) :
    m_internal_format(internal_format),
    m_width(width),
    m_height(height),
    m_format(format),
    m_type(type),
    m_filter_min(filter_min),
    m_filter_max(filter_max),
    m_clamp(clamp),
    m_shading_type(shading_type),
    m_path(path)
{
    generateTexture(data);
}

void Texture::init(const GLint &internal_format, const GLsizei &width, const GLsizei &height, const GLenum &format, const GLenum &type, const GLvoid *data, GLuint clamp, GLuint filter_min, GLuint filter_max, std::__1::string path, std::__1::string shading_type)
{
    m_internal_format = internal_format;
    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;
    m_filter_min = filter_min;
    m_filter_max = filter_max;
    m_clamp = clamp;
    m_shading_type = shading_type;
    m_path = path;

    generateTexture(data);
}

void Texture::generateTexture(const GLvoid *data)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0, m_format, m_type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter_max);
    if(m_clamp)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_clamp);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_clamp);
    }
}
