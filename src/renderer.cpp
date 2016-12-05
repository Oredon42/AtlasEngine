#include "include/renderer.h"
#include "include/scene.h"
#include "include/pointlight.h"

#include <QOpenGLFramebufferObject>

Renderer::Renderer() :
    m_width(800),
    m_height(600),
    m_rbo_depth(0),
    m_quad_VAO(0),
    m_quad_VBO(0),
    m_quad(1.f),
    m_gPosition(0),
    m_gNormal(0),
    m_gAlbedoSpec(0),
    m_SSAO_postprocess(m_quad),
    m_exposure(1.f),
    m_hdr(GL_TRUE),
    m_adaptation(GL_TRUE),
    m_bloom(GL_TRUE)
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
    setDimensions(window_width, window_height);

    /*
     * G BUFFER
     * */
    m_gBuffer.init(window_width, window_height);
    FramebufferTextureDatas gTexture_datas[3] = {FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT),       //  position + depth
                                                 FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT),         //  normal
                                                 FramebufferTextureDatas(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE)}; //  albedo

    m_gBuffer.attachTextures(gTexture_datas, 3, GL_CLAMP_TO_EDGE, GL_TRUE);

    // Setup plane for framebuffer render
    //initQuad();
    m_quad.setupBuffers();

    // Setup shaders
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        m_shader_forward[i].initForward(m_shader_types[i], nb_dirlights, nb_pointlights, nb_spotlights);
        m_shader_geometry_pass[i].initGeometry(m_shader_types[i]);
    }
    m_shader_lighting_pass.initLighting(nb_dirlights, nb_pointlights, nb_spotlights);

    m_shader_lighting_pass.use();
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "ssao"), 3);
    glUseProgram(0);

    /*
     * HDR
     * */
    m_hdr_shader.init("shaders/hdr.vert", "shaders/hdr.frag", path);
    m_hdr_shader.use();
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);
    m_hdr_buffer.init(window_width, window_height);
    FramebufferTextureDatas hdr_texture_datas[3] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT),
                                                    FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT),
                                                    FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT)};
    m_hdr_buffer.attachTextures(hdr_texture_datas, 3, GL_CLAMP_TO_BORDER, GL_TRUE);

    /*
     * BLOOM
     * */
    m_blur_shaders[0].init("shaders/blur.vert", "shaders/blurv.frag", path);
    m_blur_shaders[1].init("shaders/blur.vert", "shaders/blurh.frag", path);
    m_blur_shaders[2].init("shaders/blur.vert", "shaders/blurmipmap.frag", path);
    m_blur_buffers[0].init(window_width, window_height);
    m_blur_buffers[1].init(window_width, window_height);
    FramebufferTextureDatas bloom_texture_datas[1] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT)};
    m_blur_buffers[0].attachTextures(bloom_texture_datas, 1, GL_CLAMP_TO_EDGE);
    m_blur_buffers[1].attachTextures(bloom_texture_datas, 1, GL_CLAMP_TO_EDGE);

    /*
     * SSAO
     * */
    m_SSAO_postprocess.init(window_width, window_height);
}

/*
 * Render a scene using forward rendering
 * */
void Renderer::drawSceneForward(Scene &scene, const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height, GLboolean (&keys)[1024])
{
    glClearColor(scene.getBackgroundColor().x, scene.getBackgroundColor().y, scene.getBackgroundColor().z, 0.5f);

    /*
     * Scene drawing pass
     * */
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdr_buffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.drawForward(m_shader_forward, keys, render_time, window_width, window_height);

    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(2));
    glGenerateMipmap(GL_TEXTURE_2D);

    generateBloom();

    glViewport(0, 0, m_width, m_height);
    //  Write output image
    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_hdr_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[1].getTexture(0));

    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "hdr"), m_hdr);
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "bloom"), m_bloom);
    glUniform1f(glGetUniformLocation(m_hdr_shader.getProgram(), "exposure"), m_exposure);

    m_quad.draw();
    //drawQuad();
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

    m_SSAO_postprocess.process(m_gBuffer, scene.getCurrentCamera()->getProjection());

    /*
     * Lighting pass
     * */
    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader_lighting_pass.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(2));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_SSAO_postprocess.getTexture());

    //for(GLuint i = 0; i < scene.numberOfPointLights(); ++i)
    //    scene.sendPointLightDatas(i, m_shader_lighting_pass);

    scene.sendCameraDatas(m_shader_lighting_pass, window_width, window_height);

    m_quad.draw();
    //drawQuad();
}

void Renderer::generateBloom()
{
    /*
     * Adaptation
     * */
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(2));
    glGenerateMipmap(GL_TEXTURE_2D);

    //  Use last level of mipmap to get the average brightness of the scene
    if(m_adaptation)
    {
        GLfloat average[1];
        glGetTexImage(GL_TEXTURE_2D, m_num_levels, GL_RED, GL_FLOAT, average);

        GLfloat average_brightness = glm::min(glm::max(average[0], 0.05f), 1.f);

        GLfloat target_exposure = 0.5f / average_brightness;
        m_exposure = m_exposure + (target_exposure - m_exposure) * 0.1f;
    }

    //  Get hdr 2nd texture + horizontal blur
    glViewport(0, 0, m_width/8, m_height/8);
    m_blur_shaders[2].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[0].getBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(1));
    m_quad.draw();
    //drawQuad();

    glViewport(0, 0, m_width, m_height);
    GLuint index_buffer = 0;
    GLuint i = 8;
    while(i > 1)
    {
        //  Vertical blur
        m_blur_shaders[0].use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
        m_quad.draw();
        //drawQuad();

        i>>=1;

        //  Bilinear interpolation to size*2
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_blur_buffers[index_buffer].getBuffer());
        glBlitFramebuffer(0, 0, m_width/(i*2), m_height/(i*2), 0, 0, m_width/i, m_height/i, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //  Horizontal blur
        m_blur_shaders[1].use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
        m_quad.draw();
        //drawQuad();

        index_buffer = !index_buffer;
    }

    //  Vertical
    m_blur_shaders[0].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
    m_quad.draw();
    //drawQuad();
}

void Renderer::reloadShaders()
{
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        m_shader_forward[i].reload();
        m_shader_geometry_pass[i].reload();
    }
}

void Renderer::initQuad()
{
    m_quad_vertices[0] = -1.0f; m_quad_vertices[1] = 1.0f; m_quad_vertices[2] = 0.0f; m_quad_vertices[3] = 0.0f; m_quad_vertices[4] = 1.0f;
    m_quad_vertices[5] = -1.0f; m_quad_vertices[6] = -1.0f; m_quad_vertices[7] = 0.0f; m_quad_vertices[8] = 0.0f; m_quad_vertices[9] = 0.0f;
    m_quad_vertices[10] = 1.0f; m_quad_vertices[11] = 1.0f; m_quad_vertices[12] = 0.0f; m_quad_vertices[13] = 1.0f; m_quad_vertices[14] = 1.0f;
    m_quad_vertices[15] = 1.0f; m_quad_vertices[16] = -1.0f; m_quad_vertices[17] = 0.0f; m_quad_vertices[18] = 1.0f; m_quad_vertices[19] = 0.0f;

    glGenVertexArrays(1, &m_quad_VAO);
    glGenBuffers(1, &m_quad_VBO);
    glBindVertexArray(m_quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_quad_vertices), &m_quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
}
