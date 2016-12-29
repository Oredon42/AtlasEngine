#include "include/data/lighting/spotlight.h"
#include "include/render/shader.h"

SpotLight::SpotLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cut_off, const GLfloat &outer_cut_off) :
   PointLight(color, intensity, position),
   m_direction(direction),
   m_cut_off(cut_off),
   m_outer_cut_off(outer_cut_off)
{

}

void SpotLight::init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cut_off, const GLfloat &outer_cut_off)
{
    PointLight::init(color, intensity, position);

    m_direction = direction;
    m_color = color;
    m_cut_off = cut_off;
    m_outer_cut_off = outer_cut_off;
}

void SpotLight::sendViewDatas(const Shader &shader, const glm::mat4 &view) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glm::vec4 position = glm::vec4(view * glm::vec4(m_position, 1));
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].intensity").c_str()), m_intensity);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].direction").c_str()), m_direction.x, m_direction.y, m_direction.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].cut_off").c_str()), m_cut_off);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].outer_cut_off").c_str()), m_outer_cut_off);
}

void SpotLight::sendDatas(const Shader &shader) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].position").c_str()), m_position.x, m_position.y, m_position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].intensity").c_str()), m_intensity);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].direction").c_str()), m_direction.x, m_direction.y, m_direction.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].cut_off").c_str()), m_cut_off);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].outer_cut_off").c_str()), m_outer_cut_off);
}
