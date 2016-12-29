#ifndef DIRLIGHT_H
#define DIRLIGHT_H

#include "openglincludes.h"
#include "light.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class DirLight : public Light
{
public:
    DirLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &direction);

    void init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &direction);

    inline virtual void sendViewDatas(const Shader &shader, const glm::mat4 &view) const{sendDatas(shader);}
    virtual void sendDatas(const Shader &shader) const;

    //  Getters
    inline glm::vec3 getDirection() const{return m_direction;}

private:
    glm::vec3 m_direction;
};

#endif // DIRLIGHT_H
