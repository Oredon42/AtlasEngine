#include "include/data/lighting/spotlight.h"
#include "include/render/shader.h"


SpotLight::SpotLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cut_off, const GLfloat &outer_cut_off) :
    Light(color, intensity),
    m_position(position),
    m_direction(direction),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f),
    m_cut_off(glm::cos(glm::radians(cut_off))),
    m_outer_cut_off(glm::cos(glm::radians(outer_cut_off)))
{
    //  Init Shadow Map
    m_depth_map_buffer.init(m_depth_map_width, m_depth_map_height);
    const GLfloat border_color[] = {1.f, 1.f, 1.f, 1.f};
    std::vector<FramebufferTextureDatas> depth_map_texture_datas;
    depth_map_texture_datas.push_back(FramebufferTextureDatas(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, border_color, GL_TRUE));
    m_depth_map_buffer.attachTextures(depth_map_texture_datas);

    glm::mat4 light_projection, light_view;
    light_projection = glm::perspective(outer_cut_off, m_depth_map_aspect, m_depth_map_near, m_depth_map_far);
    light_view = glm::lookAt(m_position, m_direction, glm::vec3(0.0, 1.0, 0.0));
    m_light_space_matrix = light_projection * light_view;
}

void SpotLight::init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cut_off, const GLfloat &outer_cut_off)
{
    //PointLight::init(color, intensity, position);

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
    glm::vec3 direction = glm::transpose(glm::inverse(glm::mat3(view))) * m_direction;
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].intensity").c_str()), m_intensity);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].direction").c_str()), direction.x, direction.y, direction.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].cut_off").c_str()), m_cut_off);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].outer_cut_off").c_str()), m_outer_cut_off);
}

void SpotLight::sendDatas(Shader &shader) const
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

    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].light_space_matrix").c_str()), 1, GL_FALSE, glm::value_ptr(m_light_space_matrix));
    shader.activateNextTexture();
    glUniform1i(glGetUniformLocation(shader.getProgram(), ("spotLights[" + index + "].depth_map").c_str()), shader.getActiveTexture());
    m_depth_map_buffer.getTexture(0)->bind();
}

void SpotLight::sendShadowDatas(Shader &shader) const
{
    glUniform3f(glGetUniformLocation(shader.getProgram(), "direction"), m_direction.x, m_direction.y, m_direction.z);
    shader.activateNextTexture();
    glUniform1i(glGetUniformLocation(shader.getProgram(), "depth_map"), shader.getActiveTexture());
    m_depth_map_buffer.getTexture(0)->bind();

    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "light_space_matrix"), 1, GL_FALSE, glm::value_ptr(m_light_space_matrix));
}
