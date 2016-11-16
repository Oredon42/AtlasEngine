#include "include/pointlight.h"
#include "include/shader.h"

PointLight::PointLight() :
    m_indice(0),
    m_position(0.0f, 0.0f, 0.0f),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f),
    m_ambient(0.1f, 0.1f, 0.1f),
    m_diffuse(0.5f, 0.5f, 0.5f),
    m_specular(1.0f, 1.0f, 1.0f)
{

}

PointLight::PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic) :
    m_indice(indice),
    m_position(position),
    m_constant(constant),
    m_linear(linear),
    m_quadratic(quadratic),
    m_ambient(ambient),
    m_diffuse(diffuse),
    m_specular(specular)
{

}

PointLight::PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &diffuse, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic) :
    m_indice(indice),
    m_position(position),
    m_constant(constant),
    m_linear(linear),
    m_quadratic(quadratic),
    m_ambient(glm::vec3(0.1f, 0.1f, 0.1f)),
    m_diffuse(diffuse),
    m_specular(glm::vec3(1.0f, 1.0f, 1.0f))
{
    m_diffuse = glm::clamp(m_diffuse, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

PointLight::PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &diffuse) :
    m_indice(indice),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f),
    m_ambient(glm::vec3(0.1f, 0.1f, 0.1f)),
    m_diffuse(diffuse),
    m_specular(glm::vec3(1.0f, 1.0f, 1.0f))
{
    m_diffuse = glm::clamp(m_diffuse, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

void PointLight::init(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic)
{
    m_indice = indice;
    m_position = position;
    m_ambient = ambient;
    m_diffuse = diffuse;
    m_specular = specular;
    m_constant = constant;
    m_linear = linear;
    m_quadratic = quadratic;
}

void PointLight::sendDatas(const Shader &shader) const
{
    std::ostringstream oss;
    oss << m_indice;
    std::string indice = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].position").c_str()), m_position.x, m_position.y, m_position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].ambient").c_str()), m_ambient.x, m_ambient.y, m_ambient.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].diffuse").c_str()), m_diffuse.x, m_diffuse.y, m_diffuse.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].specular").c_str()), m_specular.x, m_specular.y, m_specular.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + indice + "].quadratic").c_str()), m_quadratic);
}
