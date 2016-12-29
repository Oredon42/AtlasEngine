#ifndef CAMERA_H
#define CAMERA_H

#include "openglincludes.h"

#include <QtGui/QKeyEvent>
#include <iostream>

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class Camera
{
public:
    Camera();
    Camera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, const GLfloat& speed, const GLfloat& fov);
    virtual ~Camera();

    void init(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, const GLfloat& speed, const GLfloat& fov);

    virtual void move(const GLboolean keys[1024], const GLfloat &delta_time);
    virtual void orientate();
    void sendDatas(const Shader &shader, const GLfloat &screen_width, const GLfloat &screen_height);

    //  Getters
    inline GLuint getViewPosLoc() const{return m_viewPosLoc;}
    inline GLuint getModelLoc() const{return m_modelLoc;}
    inline GLuint getViewLoc() const{return m_viewLoc;}
    inline GLfloat getFov() const{return m_fov;}
    inline GLfloat getSpeed() const{return m_speed;}
    inline glm::vec3 getPosition() const{return m_position;}
    inline glm::vec3 getFront() const{return m_front;}
    inline glm::vec3 getUp() const{return m_up;}
    inline glm::mat4 getView() const{return m_view;}
    inline glm::mat4 getProjection() const{return m_projection;}
    inline GLboolean isInitialised() const{return m_initialised;}

    //  Setters
    inline void setFov(const GLfloat &fov){m_fov = fov;}
    inline void setSpeed(const GLfloat &speed){m_speed = speed;}
    inline void setPosition(const glm::vec3 &position){m_position = position;}
    inline void setFront(const glm::vec3 &front){m_front = front;}
    inline void setUp(const glm::vec3 &up){m_up = up;}
    inline void setOffset(const GLfloat &xoffset, const GLfloat &yoffset){m_current_offset_x = xoffset;m_current_offset_y = yoffset;}

private:
    GLuint m_viewPosLoc;
    GLuint m_modelLoc;
    GLuint m_viewLoc;
    GLuint m_projectionLoc;

    GLfloat m_current_offset_x;
    GLfloat m_current_offset_y;
    GLfloat m_saved_offset_x;
    GLfloat m_saved_offset_y;

    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_fov;
    GLfloat m_speed;

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;

    glm::mat4 m_view;
    glm::mat4 m_projection;

    GLboolean m_initialised;
};

#endif // CAMERA_H
