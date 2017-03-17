#ifndef LIGHT_H
#define LIGHT_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "framebuffer.h"

class Shader;

class Light
{
public:
    Light(const glm::vec3 &color, const GLfloat &intensity, GLuint m_depth_map_width = 1024, GLuint depth_map_height = 1024, GLfloat depth_map_near = 1.f, GLfloat depth_map_far = 25.f);

    void init(const glm::vec3 &color, const GLfloat &intensity);

    virtual void sendViewDatas(const Shader &shader, const glm::mat4 &view) const = 0; // TODO: delete?
    virtual void sendDatas(Shader &shader) const = 0;
    virtual void sendShadowDatas(Shader &shader) const = 0;

    inline void bindDepthMapBuffer() {m_depth_map_buffer.bind();}
    inline void releaseDepthMapBuffer() {m_depth_map_buffer.release();}

    //  Getters
    inline GLuint getIndex() const{return m_index;}
    inline glm::vec3 getColor() const{return m_color;}
    inline GLfloat getIntensity() const{return m_intensity;}
    inline GLuint getDepthMapWidth() const{return m_depth_map_width;}
    inline GLuint getDepthMapHeight() const{return m_depth_map_height;}
    inline GLfloat getDepthMapFar() const{return m_depth_map_far;}

    //  Setter
    inline void setIndex(const GLuint &index){m_index = index;}

protected:
    GLuint m_index;
    glm::vec3 m_color;
    GLfloat m_intensity;

    // Shadowmap
    Framebuffer m_depth_map_buffer;
    GLuint m_depth_map_width;
    GLuint m_depth_map_height;
    GLfloat m_depth_map_near;
    GLfloat m_depth_map_far;
    GLfloat m_depth_map_aspect;
};

#endif // LIGHT_H
