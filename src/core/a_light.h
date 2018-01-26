#ifndef A_LIGHT_H
#define A_LIGHT_H

#include <glew.h>
#include <glm/glm.hpp>

class Framebuffer;
class Shader;

class ALight
{
public:
    virtual ~ALight();

    inline void bindDepthMapBuffer();
    inline void releaseDepthMapBuffer();

    //  Getters
    inline glm::vec3 getColor() const{return m_vColor;}
    inline GLfloat getIntensity() const{return m_fIntensity;}
    inline GLuint getDepthMapWidth() const{return m_uiDepthMapWidth;}
    inline GLuint getDepthMapHeight() const{return m_uiDepthMapHeight;}
    inline GLfloat getDepthMapFar() const{return m_fDepthMapFar;}

protected:
    //  Protected constructor
    ALight(const glm::vec3 &vColor, const GLfloat &fIntensity, const GLuint &uiDepthMapWidth = 1024, const GLuint &uiDepthMapHeight = 1024, const GLfloat &fDepthMapNear = 1.f, const GLfloat &fDepthMapFar = 25.f);

    glm::vec3 m_vColor;
    GLfloat m_fIntensity;

    // Shadowmap
    Framebuffer *m_pDepthMapBuffer;
    GLuint m_uiDepthMapWidth;
    GLuint m_uiDepthMapHeight;
    GLfloat m_fDepthMapNear;
    GLfloat m_fDepthMapFar;
    GLfloat m_fDepthMapAspect;
};

#endif // A_LIGHT_H
