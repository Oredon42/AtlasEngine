#include "include/data/pointlight.h"
#include "include/render/shader.h"

PointLight::PointLight() :
    m_indice(0),
    m_position(0.0f, 0.0f, 0.0f),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f),
    m_intensity(1.f),
    m_color(1.f, 0.f, 1.f)
{

}

PointLight::PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &color, const GLfloat &intensity) :
    m_indice(indice),
    m_position(position),
    m_color(color),
    m_intensity(intensity),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f)
{
    m_color = glm::clamp(m_color, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

void PointLight::init(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &color, const GLfloat &intensity)
{
    m_indice = indice;
    m_position = position;
    m_color = color;
}

void PointLight::sendViewDatas(const Shader &shader, const glm::mat4 &view) const
{
    std::ostringstream oss;
    oss << m_indice;
    std::string indice = oss.str();
    glm::vec4 position = glm::vec4(view * glm::vec4(m_position, 1));
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].intensity").c_str()), m_intensity);
}

void PointLight::sendDatas(const Shader &shader) const
{
    std::ostringstream oss;
    oss << m_indice;
    std::string indice = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].position").c_str()), m_position.x, m_position.y, m_position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].intensity").c_str()), m_intensity);
}
