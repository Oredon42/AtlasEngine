#include "include/data/lighting/pointlight.h"
#include "include/render/shader.h"


PointLight::PointLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position) :
    Light(color, intensity),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f)
{
    //  Init Shadow Map
    m_depth_map_buffer.init(m_depth_map_width, m_depth_map_height);
    const GLfloat border_color[] = {1.f, 1.f, 1.f, 1.f};
    std::vector<FramebufferTextureDatas> depth_map_texture_datas;
    depth_map_texture_datas.push_back(FramebufferTextureDatas(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, border_color, GL_TRUE, GL_TEXTURE_CUBE_MAP));
    m_depth_map_buffer.attachTextures(depth_map_texture_datas);


    glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), m_depth_map_aspect, m_depth_map_near, m_depth_map_far);
    m_shadow_transforms[0] = glm::mat4(shadow_proj * glm::lookAt(m_position, m_position + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    m_shadow_transforms[1] = glm::mat4(shadow_proj * glm::lookAt(m_position, m_position + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    m_shadow_transforms[2] = glm::mat4(shadow_proj * glm::lookAt(m_position, m_position + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
    m_shadow_transforms[3] = glm::mat4(shadow_proj * glm::lookAt(m_position, m_position + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
    m_shadow_transforms[4] = glm::mat4(shadow_proj * glm::lookAt(m_position, m_position + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
    m_shadow_transforms[5] = glm::mat4(shadow_proj * glm::lookAt(m_position, m_position + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));
}

void PointLight::init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position)
{
    Light::init(color, intensity);

    m_position = position;
}

void PointLight::sendViewDatas(const Shader &shader, const glm::mat4 &view) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glm::vec4 position = glm::vec4(view * glm::vec4(m_position, 1));
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].intensity").c_str()), m_intensity);

    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].depth_map_near").c_str()), m_depth_map_near);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].depth_map_far").c_str()), m_depth_map_far);
    glUniform1i(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].depth_map").c_str()), m_depth_map_buffer.getTexture(0)->getId());
}

void PointLight::sendDatas(Shader &shader) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].position").c_str()), m_position.x, m_position.y, m_position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].intensity").c_str()), m_intensity);

    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].depth_map_near").c_str()), m_depth_map_near);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].depth_map_far").c_str()), m_depth_map_far);
    shader.activateNextTexture();
    glUniform1i(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].depth_map").c_str()), shader.getActiveTexture());
    m_depth_map_buffer.getTexture(0)->bind();

    for(GLuint i = 0; i < 6; ++i)
        glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(m_shadow_transforms[i]));
}

void PointLight::sendShadowDatas(Shader &shader) const
{
    glUniform3f(glGetUniformLocation(shader.getProgram(), "position"), m_position.x, m_position.y, m_position.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "depth_map_near"), m_depth_map_near);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "depth_map_far"), m_depth_map_far);
    shader.activateNextTexture();
    glUniform1i(glGetUniformLocation(shader.getProgram(), "depth_map"), shader.getActiveTexture());
    m_depth_map_buffer.getTexture(0)->bind();

    for(GLuint i = 0; i < 6; ++i)
        glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), ("shadow_matrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(m_shadow_transforms[i]));
}
