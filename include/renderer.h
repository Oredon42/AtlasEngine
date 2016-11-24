#ifndef RENDERER_H
#define RENDERER_H

#include <string>

#ifndef OGL_DEF
#define OGL_DEF
#ifdef __APPLE__
    #include <QGLWidget>
#elif __linux__
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#elif _WIN32
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif
#endif

#include "shader.h"

class Scene;

class Renderer
{
public:
    Renderer();

    void init(const std::string &path, const GLuint &window_width, const GLuint &window_height, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);
    void drawSceneForward(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024]);
    void drawSceneDeffered(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024]);

    void reloadShaders();

private:
    GLuint m_default_buffer_binding;

    GLuint m_gBuffer;
    GLuint m_rbo_depth;
    GLuint m_quad_VAO, m_quad_VBO;
    GLfloat m_quad_vertices[20];
    Shader m_shader_lightning_pass;
    Shader m_shader_geometry_pass[NB_SHADER_TYPES];

    Shader m_shader_forward[NB_SHADER_TYPES];
    ShaderType m_shader_types[NB_SHADER_TYPES];

    GLuint m_HDR_buffer;
    GLuint m_color_buffer;

    GLuint m_gPosition, m_gNormal, m_gAlbedoSpec;
};

#endif // RENDERER_H
