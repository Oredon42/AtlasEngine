#ifndef TEXTURE_H
#define TEXTURE_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Texture
{
public:
    Texture();
    Texture(const GLint &internal_format, const GLsizei &width, const GLsizei &height, const GLenum &format, const GLenum &type, const GLvoid *data, GLuint clamp = GL_FALSE, GLuint filter_min = GL_NEAREST, GLuint filter_max = GL_NEAREST, std::string path = "", std::string shading_type = "");
    void init(const GLint &internal_format, const GLsizei &width, const GLsizei &height, const GLenum &format, const GLenum &type, const GLvoid *data, GLuint clamp = GL_FALSE, GLuint filter_min = GL_NEAREST, GLuint filter_max = GL_NEAREST, std::string path = "", std::string shading_type = "");

    void generateTexture(const GLvoid *data);
    void updateDimensions(const GLsizei width, const GLsizei height);
    inline void generateMipmaps(){glBindTexture(GL_TEXTURE_2D, m_id);glGenerateMipmap(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D, 0);}

    //  Getters
    inline GLuint getId()const {return m_id;}
    inline std::string getShadingType()const {return m_shading_type;}
    inline std::string getPath()const {return m_path;}

private:
    GLuint m_id;
    GLint m_internal_format;
    GLsizei m_width;
    GLsizei m_height;
    GLenum m_format;
    GLenum m_type;
    GLuint m_filter_min;
    GLuint m_filter_max;
    GLuint m_clamp;
    std::string m_shading_type;
    std::string m_path;
};

#endif // TEXTURE_H
