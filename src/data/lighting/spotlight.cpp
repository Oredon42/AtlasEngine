#include "include/data/lighting/spotlight.h"
#include "include/render/shader.h"

SpotLight::SpotLight() :
m_position(0.0f, 0.0f, 0.0f),
m_direction(0.0f, 0.0f, -1.0f),
m_cutOff(glm::cos(glm::radians(12.5f))),
m_outerCutOff(glm::cos(glm::radians(15.0f))),
m_constant(1.0f),
m_linear(0.7f),
m_quadratic(1.8f),
m_ambient(0.1f, 0.1f, 0.1f),
m_diffuse(0.5f, 0.5f, 0.5f),
m_specular(1.0f, 1.0f, 1.0f)
{

}

SpotLight::SpotLight(const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cutOff, const GLfloat &outerCutOff, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic) :
m_position(position),
m_direction(direction),
m_cutOff(cutOff),
m_outerCutOff(outerCutOff),
m_constant(constant),
m_linear(linear),
m_quadratic(quadratic),
m_ambient(ambient),
m_diffuse(diffuse),
m_specular(specular)
{

}

void SpotLight::init(const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cutOff, const GLfloat &outerCutOff, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic)
{
    m_position = position;
    m_direction = direction;
    m_cutOff = cutOff;
    m_outerCutOff = outerCutOff;
    m_ambient = ambient;
    m_diffuse = diffuse;
    m_specular = specular;
    m_constant = constant;
    m_linear = linear;
    m_quadratic = quadratic;
}

void SpotLight::sendDatas(const Shader &shader) const
{
    glUniform3f(glGetUniformLocation(shader.getProgram(), "spotLight.position"), m_position.x, m_position.y, m_position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "spotLight.direction"), m_direction.x, m_direction.y, m_direction.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "spotLight.ambient"), m_ambient.x, m_ambient.y, m_ambient.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "spotLight.diffuse"), m_diffuse.x, m_diffuse.y, m_diffuse.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), "spotLight.specular"), m_specular.x, m_specular.y, m_specular.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "spotLight.constant"), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "spotLight.linear"), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "spotLight.quadratic"), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "spotLight.cutOff"), m_cutOff);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "spotLight.outerCutOff"), m_outerCutOff);
}

void SpotLight::updatePosDir(const glm::vec3 &position, const glm::vec3 &direction)
{
    m_position = position;
    m_direction = direction;
}
