#include "include/renderer.h"
#include "include/scene.h"
#include "include/pointlight.h"

Renderer::Renderer() :
    m_gBuffer(0),
    m_rbo_depth(0),
    m_quad_VAO(0),
    m_quad_VBO(0),
    m_gPosition(0),
    m_gNormal(0),
    m_gAlbedoSpec(0)
{
    m_shader_types[0].setValues(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 0);
    m_shader_types[1].setValues(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, 1);
    m_shader_types[2].setValues(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE, 2);
    m_shader_types[3].setValues(GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, 3);
    m_shader_types[4].setValues(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, 4);
    m_shader_types[5].setValues(GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE, 5);
    m_shader_types[6].setValues(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE, 6);
    m_shader_types[7].setValues(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE, 7);

    m_shader_types[8].setValues(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE, 8);
    m_shader_types[9].setValues(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, 9);
    m_shader_types[10].setValues(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, 10);
    m_shader_types[11].setValues(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE, 11);
    m_shader_types[12].setValues(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE, 12);
    m_shader_types[13].setValues(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE, 13);
    m_shader_types[14].setValues(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE, 14);
    m_shader_types[15].setValues(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, 15);
}

void Renderer::init(const std::string &path, const GLuint &window_width, const GLuint &window_height, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights)
{
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    // - Position color buffer
    glGenTextures(1, &m_gPosition);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

    // - Normal color buffer
    glGenTextures(1, &m_gNormal);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &m_gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &m_rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_depth);

    // - Finally check if framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_quad_vertices[0] = -1.0f; m_quad_vertices[1] = 1.0f; m_quad_vertices[2] = 0.0f; m_quad_vertices[3] = 0.0f; m_quad_vertices[4] = 1.0f;
    m_quad_vertices[5] = -1.0f; m_quad_vertices[6] = -1.0f; m_quad_vertices[7] = 0.0f; m_quad_vertices[8] = 0.0f; m_quad_vertices[9] = 0.0f;
    m_quad_vertices[10] = 1.0f; m_quad_vertices[11] = 1.0f; m_quad_vertices[12] = 0.0f; m_quad_vertices[13] = 1.0f; m_quad_vertices[14] = 1.0f;
    m_quad_vertices[15] = 1.0f; m_quad_vertices[16] = -1.0f; m_quad_vertices[17] = 0.0f; m_quad_vertices[18] = 1.0f; m_quad_vertices[19] = 0.0f;

    // Setup plane VAO
    glGenVertexArrays(1, &m_quad_VAO);
    glGenBuffers(1, &m_quad_VBO);
    glBindVertexArray(m_quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_quad_vertices), &m_quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    // Set up shaders
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        m_shader_forward[i].init(m_shader_types[i], RenderingMethod::FORWARD, nb_dirlights, nb_pointlights, nb_spotlights);
        m_shader_geometry_pass[i].init(m_shader_types[i], RenderingMethod::DEFFERED, nb_dirlights, nb_pointlights, nb_spotlights);
    }
    m_shader_lightning_pass.init("/shaders/deffered_shading.vert", "/shaders/deffered_shading.frag", path);

    m_shader_lightning_pass.use();
    glUniform1i(glGetUniformLocation(m_shader_lightning_pass.getProgram(), "gPosition"), 0);
    glUniform1i(glGetUniformLocation(m_shader_lightning_pass.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_shader_lightning_pass.getProgram(), "gAlbedoSpec"), 2);
    glUseProgram(0);
}

/*
 * Render a scene using forward rendering
 * */
void Renderer::drawSceneForward(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024])
{
    scene.drawForward(m_shader_forward, keys, render_time, window_width, window_height);
}

/*
 * Render a scene using deferred rendering
 * */
void Renderer::drawSceneDeffered(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024])
{
    GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
    // GEOMETRY PASS
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.drawDeferred(m_shader_geometry_pass, keys, render_time, window_width, window_height);

    glBindFramebuffer(GL_FRAMEBUFFER, 2);
    //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);

    // LIGHTNING PASS
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader_lightning_pass.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);

    // Also send light relevant uniforms
    for(GLuint i = 0; i < scene.numberOfPointLights(); ++i)
        scene.sendPointLightDatas(i, m_shader_lightning_pass);

    scene.sendCameraDatas(m_shader_lightning_pass, window_width, window_height);

    // Finally render quad
    glBindVertexArray(m_quad_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::reloadShaders()
{
    std::cout << "reload" << std::endl;
    //m_shader_geometry_pass.reload();
    m_shader_lightning_pass.reload();
}
