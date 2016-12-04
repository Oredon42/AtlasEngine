#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include <math.h>

#include "openglincludes.h"

#include "shader.h"
#include "framebuffer.h"

inline GLfloat lerp(GLfloat a, GLfloat b, GLfloat f){return a + f * (b - a);}

class Scene;

class Renderer
{
public:
    Renderer();

    void init(const std::string &path, const GLuint &window_width, const GLuint &window_height, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);
    void drawSceneForward(Scene &scene, const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height, GLboolean (&keys)[1024]);
    void drawSceneDeffered(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024]);

    void reloadShaders();

    void generateBloom();
    void generateSSAO(const Scene &scene);

    inline void drawQuad()const {glBindVertexArray(m_quad_VAO);glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);glBindVertexArray(0);}

    inline void setDimensions(const GLuint &width, const GLuint &height) {m_width = width; m_height = height; m_num_levels = floor(log2(std::max(m_width, m_height)));}
    inline void switchHDR(){m_hdr = !m_hdr;}
    inline void switchBloom(){m_bloom = !m_bloom;}
    inline void switchAdaptation(){m_adaptation = !m_adaptation;m_exposure = 1.f;}

private:
    void initQuad();

    GLuint m_width;
    GLuint m_height;
    GLuint m_num_levels;

    Framebuffer m_gBuffer;
    GLuint m_rbo_depth;
    GLuint m_quad_VAO, m_quad_VBO;
    GLfloat m_quad_vertices[20];
    Shader m_shader_lighting_pass;
    Shader m_shader_geometry_pass[NB_SHADER_TYPES];

    Shader m_shader_forward[NB_SHADER_TYPES];
    ShaderType m_shader_types[NB_SHADER_TYPES];

    //  HDR
    Framebuffer m_hdr_buffer;
    Shader m_hdr_shader;
    GLuint m_color_buffer;
    GLfloat m_exposure;
    GLboolean m_hdr;
    GLboolean m_adaptation;

    //  BLOOM
    Framebuffer m_blur_buffers[2];
    Shader m_blur_shaders[3];
    GLboolean m_bloom;

    //  SSAO
    Framebuffer m_SSAO_buffer;
    Framebuffer m_SSAO_blur_buffer;
    Shader m_SSAO_shader;
    Shader m_SSAO_blur_shader;
    GLboolean m_SSAO;
    Texture m_noise_texture;
    std::vector<glm::vec3> m_ssao_kernel;

    GLuint m_gPosition, m_gNormal, m_gAlbedoSpec;
};

#endif // RENDERER_H
