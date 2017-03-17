#include "include/data/lighting/dirlight.h"
#include "include/render/shader.h"

DirLight::DirLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &direction) :
    Light(color, intensity),
    m_direction(direction)
{
    //  Init Shadow Map
    m_depth_map_buffer.init(m_depth_map_width, m_depth_map_height);
    const GLfloat border_color[] = {1.f, 1.f, 1.f, 1.f};
    std::vector<FramebufferTextureDatas> depth_map_texture_datas;
    depth_map_texture_datas.push_back(FramebufferTextureDatas(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, border_color, GL_TRUE));
    m_depth_map_buffer.attachTextures(depth_map_texture_datas);

    glm::mat4 light_projection, light_view;
    light_projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, m_depth_map_near, m_depth_map_far);
    light_view = glm::lookAt(-m_direction * 12.f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    m_light_space_matrix = light_projection * light_view;
}

void DirLight::init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &direction)
{
    Light::init(color, intensity);

    m_direction = direction;
}

void DirLight::sendDatas(Shader &shader) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].intensity").c_str()), m_intensity);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].direction").c_str()), m_direction.x, m_direction.y, m_direction.z);

    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].light_space_matrix").c_str()), 1, GL_FALSE, glm::value_ptr(m_light_space_matrix));
    shader.activateNextTexture();
    glUniform1i(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].depth_map").c_str()), shader.getActiveTexture());
    m_depth_map_buffer.getTexture(0)->bind();
}

void DirLight::sendViewDatas(const Shader &shader, const glm::mat4 &view) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glm::vec3 direction = glm::transpose(glm::inverse(glm::mat3(view))) * m_direction;
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].intensity").c_str()), m_intensity);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("dirLights[" + index + "].direction").c_str()), direction.x, direction.y, direction.z);
}

void DirLight::sendShadowDatas(Shader &shader) const
{
    glUniform3f(glGetUniformLocation(shader.getProgram(), "direction"), m_direction.x, m_direction.y, m_direction.z);
    shader.activateNextTexture();
    glUniform1i(glGetUniformLocation(shader.getProgram(), "depth_map"), shader.getActiveTexture());
    m_depth_map_buffer.getTexture(0)->bind();

    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "light_space_matrix"), 1, GL_FALSE, glm::value_ptr(m_light_space_matrix));
}
