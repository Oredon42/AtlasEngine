#ifndef RENDERER_H
#define RENDERER_H

#include <string>

#include "openglincludes.h"

#include "shader.h"
#include "framebuffer.h"

class Scene;

class Renderer
{
public:
    Renderer();

    void init(const std::string &path, const GLuint &window_width, const GLuint &window_height, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);
    void drawSceneForward(Scene &scene, const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height, GLboolean (&keys)[1024]);
    void drawSceneDeffered(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024]);

    void reloadShaders();

    inline void switchHDR(){m_hdr = !m_hdr;}
    inline void switchBloom(){m_bloom = !m_bloom;}

private:
    GLint m_default_buffer_binding;

    Framebuffer m_gBuffer;
    GLuint m_rbo_depth;
    GLuint m_quad_VAO, m_quad_VBO;
    GLfloat m_quad_vertices[20];
    Shader m_shader_lightning_pass;
    Shader m_shader_geometry_pass[NB_SHADER_TYPES];

    Shader m_shader_forward[NB_SHADER_TYPES];
    ShaderType m_shader_types[NB_SHADER_TYPES];

    Framebuffer m_hdr_buffer;
    Shader m_hdr_shader;
    GLuint m_color_buffer;
    GLfloat m_exposure;
    GLboolean m_hdr;

    Framebuffer m_blur_buffers[2];
    Shader m_blur_shaders[3];
    GLboolean m_bloom;

    GLuint m_gPosition, m_gNormal, m_gAlbedoSpec;
};

#endif // RENDERER_H
