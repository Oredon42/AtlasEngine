#include "include/data/lighting/dirlight.h"
#include "include/render/shader.h"

DirLight::DirLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &direction) :
    Light(color, intensity),
    m_direction(direction)
{

}

void DirLight::init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &direction)
{
    Light::init(color, intensity);

    m_direction = direction;
}

void DirLight::sendDatas(const Shader &shader) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].intensity").c_str()), m_intensity);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].direction").c_str()), m_direction.x, m_direction.y, m_direction.z);
}
