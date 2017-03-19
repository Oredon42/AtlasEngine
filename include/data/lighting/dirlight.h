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

    virtual void sendDatas(Shader &shader) const;
    virtual void sendViewDatas(const Shader &shader, const glm::mat4 &view) const;
    virtual void sendShadowDatas(Shader &shader) const;

    //  Getters
    inline glm::vec3 getDirection() const{return m_direction;}

private:
    glm::vec3 m_direction;
    glm::mat4 m_light_space_matrix;
};

#endif // DIRLIGHT_H
