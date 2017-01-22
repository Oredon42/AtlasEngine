#include "include/render/process/shadowmaprenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

ShadowMapRenderProcess::ShadowMapRenderProcess(const GLuint nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights) :
    RenderProcess::RenderProcess(1),
    m_shadow_width(1024),
    m_shadow_height(1024),
    m_near_plane(1.f),
    m_far_plane(10.f)
{

}

void ShadowMapRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(width, height);

    m_dirlights_depth_maps_buffer.init(m_shadow_width, m_shadow_height);
    std::vector<FramebufferTextureDatas> depth_map_texture_datas;
    depth_map_texture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_NEAREST, GL_NEAREST, {1.f, 1.f, 1.f, 1.f}));
    m_dirlights_depth_maps_buffer.attachTextures(depth_map_texture_datas);

    m_shader.init("shaders/depth.vert", "shaders/depth.frag");

    m_out_textures.push_back(m_dirlights_depth_maps_buffer.getTexture(0));
}

void ShadowMapRenderProcess::initMenuElement()
{

}

void ShadowMapRenderProcess::resize(const GLuint &width, const GLuint &height)
{

}

void ShadowMapRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    glm::mat4 light_projection, light_view;
    glm::mat4 light_space_matrix;

    light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_near_plane, m_far_plane);
    //lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
    light_view = glm::lookAt(glm::vec3(1.f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    light_space_matrix = light_projection * light_view;

    m_shader.use();
    glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgram(), "light_space_matrix"), 1, GL_FALSE, glm::value_ptr(light_space_matrix));

    glViewport(0, 0, m_shadow_width, m_shadow_height);
    m_dirlights_depth_maps_buffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    scene.draw(m_shader, keys, render_time, m_shadow_width, m_shadow_height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
