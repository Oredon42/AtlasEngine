#ifndef A_MOTION_H
#define A_MOTION_H

#include <glew.h>
#include <glm/glm.hpp>

//  TODO

class Shader;

class AMotion
{
public:
    AMotion();
    AMotion(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp, const float &fSpeed, const float &fFov);
    virtual ~AMotion();

    void init(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp, const float &fSpeed, const float &fFov);

    virtual void move(const float &fRenderTime, const GLboolean (&keys)[1024]);
    virtual void orientate();
    void sendDatas(const Shader &shader, const GLfloat &fScreenWidth, const GLfloat &fScreenHeight);

    //  Getters
    inline float getFov() const{return m_fFov;}
    inline float getSpeed() const{return m_fSpeed;}
    inline glm::vec3 getPosition() const{return m_vPosition;}
    inline glm::vec3 getFront() const{return m_vFront;}
    inline glm::vec3 getUp() const{return m_vUp;}
    inline glm::mat4 getView() const{return m_mView;}
    inline glm::mat3 getTransposedInvertedView() const{return m_mTransposedInvertedView;}
    inline glm::mat4 getProjection() const{return m_mProjection;}

    //  Setters
    inline void setFov(const float &fFov){m_fFov = fFov;}
    inline void setSpeed(const float &fSpeed){m_fSpeed = fSpeed;}
    inline void setPosition(const glm::vec3 &position){m_vPosition = position;}
    inline void setFront(const glm::vec3 &vFront){m_vFront = vFront;}
    inline void setUp(const glm::vec3 &vUp){m_vUp = vUp;}
    inline void setOffset(const float &fXOffset, const GLfloat &fYOffset){m_fCurrentOffsetX = fXOffset; m_fCurrentOffsetY = fYOffset;}
    inline void setNear(const GLfloat &fNear){m_fNear = fmin(fNear, m_fFar);}
    inline void setFar(const GLfloat &fFar){m_fFar = fmax(m_fNear, fFar);}

private:
    float m_fCurrentOffsetX;
    float m_fCurrentOffsetY;
    float m_fSavedOffsetX;
    float m_fSavedOffsetY;

    float m_fYaw;
    float m_fPitch;
    float m_fSpeed;
    float m_fSensitivity;
};

#endif // A_MOTION_H
