#include "include/data/dirlight.h"
#include "include/render/shader.h"

DirLight::DirLight() :
m_direction(0.0f, 0.0f, 0.0f),
m_ambient(0.1f, 0.1f, 0.1f),
m_diffuse(0.5f, 0.5f, 0.5f),
m_specular(1.0f, 1.0f, 1.0f)
{

}

DirLight::DirLight(const glm::vec3 &direction, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular) :
m_direction(direction),
m_ambient(ambient),
m_diffuse(diffuse),
m_specular(specular)
{

}

void DirLight::init(const glm::vec3 &direction, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular)
{
    m_direction = direction;
    m_ambient = ambient;
    m_diffuse = diffuse;
    m_specular = specular;
}

void DirLight::sendDatas(const Shader &shader) const
{
    glUniform3f(glGetUniformLocation(shader.getProgram(), "dirLight.direction"), m_direction.x, m_direction.y, m_direction.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "dirLight.ambient"), m_ambient.x, m_ambient.y, m_ambient.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "dirLight.diffuse"), m_diffuse.x, m_diffuse.y, m_diffuse.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "dirLight.specular"), m_specular.x, m_specular.y, m_specular.z);
}
