#include "include/data/lighting/light.h"

Light::Light(const glm::vec3 &color, const GLfloat &intensity, GLuint depth_map_width, GLuint depth_map_height, GLfloat depth_map_near, GLfloat depth_map_far) :
    m_index(0),
    m_color(color),
    m_intensity(intensity),
    m_depth_map_width(depth_map_width),
    m_depth_map_height(depth_map_height),
    m_depth_map_near(depth_map_near),
    m_depth_map_far(depth_map_far)
{
    m_color = glm::clamp(m_color, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
    m_depth_map_aspect = m_depth_map_width / m_depth_map_height;
}

void Light::init(const glm::vec3 &color, const GLfloat &intensity)
{
    m_color = color;
    m_intensity = intensity;
}
