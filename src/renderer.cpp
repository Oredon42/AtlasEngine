#include "include/renderer.h"
#include "include/scene.h"
#include "include/pointlight.h"

#include <QOpenGLFramebufferObject>

Renderer::Renderer() :
    m_rbo_depth(0),
    m_quad_VAO(0),
    m_quad_VBO(0),
    m_gPosition(0),
    m_gNormal(0),
    m_exposure(1.f),
    m_hdr(GL_TRUE),
    m_bloom(GL_TRUE),
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
    /*
     * G BUFFER
     * */
    m_gBuffer.init(window_width, window_height);

    FramebufferTextureDatas gTexture_datas[3] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT),         //  position
                                                 FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT),         //  normal
                                                 FramebufferTextureDatas(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE)}; //  albedo

    m_gBuffer.attachTextures(gTexture_datas, 3);


    m_quad_vertices[0] = -1.0f; m_quad_vertices[1] = 1.0f; m_quad_vertices[2] = 0.0f; m_quad_vertices[3] = 0.0f; m_quad_vertices[4] = 1.0f;
    m_quad_vertices[5] = -1.0f; m_quad_vertices[6] = -1.0f; m_quad_vertices[7] = 0.0f; m_quad_vertices[8] = 0.0f; m_quad_vertices[9] = 0.0f;
    m_quad_vertices[10] = 1.0f; m_quad_vertices[11] = 1.0f; m_quad_vertices[12] = 0.0f; m_quad_vertices[13] = 1.0f; m_quad_vertices[14] = 1.0f;
    m_quad_vertices[15] = 1.0f; m_quad_vertices[16] = -1.0f; m_quad_vertices[17] = 0.0f; m_quad_vertices[18] = 1.0f; m_quad_vertices[19] = 0.0f;

    // Setup plane for framebuffer render
    glGenVertexArrays(1, &m_quad_VAO);
    glGenBuffers(1, &m_quad_VBO);
    glBindVertexArray(m_quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_quad_vertices), &m_quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    // Setup shaders
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

    /*
     * HDR
     * */
    m_hdr_shader.init("/shaders/hdr.vert", "/shaders/hdr.frag", path);
    m_hdr_shader.use();
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);
    m_hdr_buffer.init(window_width, window_height);
    FramebufferTextureDatas hdr_texture_datas[2] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT),
                                                    FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT)};
    m_hdr_buffer.attachTextures(hdr_texture_datas, 2, GL_TRUE);

    /*
     * BLOOM
     * */
    m_blur_shaders[0].init("/shaders/blur.vert", "/shaders/blurv.frag", path);
    m_blur_shaders[1].init("/shaders/blur.vert", "/shaders/blurh.frag", path);
    m_blur_shaders[2].init("/shaders/blur.vert", "/shaders/blurmipmap.frag", path);
    m_blur_buffers[0].init(window_width, window_height);
    m_blur_buffers[1].init(window_width, window_height);
    FramebufferTextureDatas bloom_texture_datas[1] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT)};
    m_blur_buffers[0].attachTextures(bloom_texture_datas, 1, GL_TRUE, GL_FALSE);
    m_blur_buffers[1].attachTextures(bloom_texture_datas, 1, GL_TRUE, GL_FALSE);
}

/*
 * Render a scene using forward rendering
 * */
void Renderer::drawSceneForward(Scene &scene, const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height, GLboolean (&keys)[1024])
{
    GLfloat background_brightness = 0.2126*scene.getBackgroundColor().x + 0.7152*scene.getBackgroundColor().y + 0.0722*scene.getBackgroundColor().z;
    background_brightness=(background_brightness < 0.3f)?0.3f:(background_brightness > 1.f)?1.f:background_brightness; // clamp
    glClearColor(scene.getBackgroundColor().x, scene.getBackgroundColor().y, scene.getBackgroundColor().z, background_brightness);

    /*
     * Scene drawing pass
     * */
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdr_buffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.drawForward(m_shader_forward, keys, render_time, window_width, window_height);

    /*
     * Generate mipmaps of 2nd texture
     * */
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(1));
    glGenerateMipmap(GL_TEXTURE_2D);

    //  Use last level of mipmap to get the average brightness of the scene
    GLint num_levels = floor(log2(std::max(window_width, window_height)));
    GLfloat average[4];
    glGetTexImage(GL_TEXTURE_2D, num_levels, GL_RGBA, GL_FLOAT, average);

    GLfloat average_brightness = average[3];
    GLfloat target_exposure = 0.5f / average_brightness;

    m_exposure = m_exposure + (target_exposure - m_exposure) * 0.1f;


    /*
     * Bloom
     * */
    glViewport(0, 0, window_width/8, window_height/8);
    m_blur_shaders[2].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[0].getBuffer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(1));

    glBindVertexArray(m_quad_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    //GLuint amount = 1;

    for(GLuint i = 8; i >= 1; i=i/2)
    {
        glViewport(0, 0, window_width, window_height);
        m_blur_shaders[0].use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[1].getBuffer());
        //glUniform1i(glGetUniformLocation(m_blur_shaders[0].getProgram(), "horizontal"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[0].getTexture(0));
        //glBindTexture(GL_TEXTURE_2D, first_iteration ? m_hdr_buffer.getTexture(1) : m_blur_buffers[!horizontal].getTexture(0));

        glBindVertexArray(m_quad_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        m_blur_shaders[1].use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[0].getBuffer());
        glUniform1i(glGetUniformLocation(m_blur_shaders[0].getProgram(), "size"), i);
        //glUniform1i(glGetUniformLocation(m_blur_shaders[0].getProgram(), "horizontal"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[1].getTexture(0));

        glBindVertexArray(m_quad_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    glViewport(0, 0, window_width, window_height);
    //  Write output image
    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_hdr_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[1].getTexture(0));

    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "hdr"), m_hdr);//hdr);
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "bloom"), m_bloom);//bloom);
    glUniform1f(glGetUniformLocation(m_hdr_shader.getProgram(), "exposure"), m_exposure);

    glBindVertexArray(m_quad_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

/*
 * Render a scene using deferred rendering
 * */
void Renderer::drawSceneDeffered(Scene &scene, const GLfloat &render_time, const GLfloat &window_width, const GLfloat &window_height, GLboolean (&keys)[1024])
{
    /*
     * Geometry pass
     * */
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.drawDeferred(m_shader_geometry_pass, keys, render_time, window_width, window_height);

    glBindFramebuffer(GL_FRAMEBUFFER, m_default_buffer_binding);

    /*
     * Lightning pass
     * */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader_lightning_pass.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(2));

    for(GLuint i = 0; i < scene.numberOfPointLights(); ++i)
        scene.sendPointLightDatas(i, m_shader_lightning_pass);

    scene.sendCameraDatas(m_shader_lightning_pass, window_width, window_height);

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
