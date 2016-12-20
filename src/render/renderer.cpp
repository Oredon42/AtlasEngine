#include "include/render/renderer.h"
#include "include/data/scene.h"
#include "include/data/pointlight.h"

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
    m_HDR_postprocess(m_quad),
    m_SSAO_postprocess(m_quad)
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
    m_quad.setupBuffers();

    /*
     * G BUFFER
     * */
    m_gBuffer.init(window_width, window_height);
    std::vector<FramebufferTextureDatas> gTexture_datas;
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE));
    m_gBuffer.attachTextures(gTexture_datas);
    std::vector<FramebufferRenderbufferDatas> gRenderbuffer_datas;
    gRenderbuffer_datas.push_back(FramebufferRenderbufferDatas(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT));
    m_gBuffer.attachRenderBuffers(gRenderbuffer_datas);

    // Setup shaders
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        m_shader_forward[i].initForward(m_shader_types[i], nb_dirlights, nb_pointlights, nb_spotlights);
        m_shader_geometry_pass[i].initGeometry(m_shader_types[i]);
    }


    m_lighting_buffer.init(m_width, m_height);
    std::vector<FramebufferTextureDatas> lighting_textures_datas;
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER));
    m_lighting_buffer.attachTextures(lighting_textures_datas);

    m_shader_lighting_pass.initLighting(nb_dirlights, nb_pointlights, nb_spotlights);
    m_shader_lighting_pass.use();
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(m_shader_lighting_pass.getProgram(), "ssao"), 3);
    glUseProgram(0);

    m_quad_shader.init("shaders/quad.vert", "shaders/quad.frag");

    m_HDR_postprocess.init(window_width, window_height);
    m_SSAO_postprocess.init(window_width, window_height);
}

void Renderer::resize(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;

    m_SSAO_postprocess.resize(m_width, m_height);
    m_HDR_postprocess.resize(m_width, m_height);

    m_gBuffer.resize(m_width, m_height);
    m_lighting_buffer.resize(m_width, m_height);

    m_SSAO_postprocess.setQuality(MEDIUM);
}

/*
 * Render a scene using deferred rendering
 * */
void Renderer::drawScene(Scene &scene, const GLfloat &render_time, GLboolean (&keys)[1024])
{
    /*
     * Geometry pass
     * */
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.getBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.draw(m_shader_geometry_pass, keys, render_time, m_width, m_height);

    m_SSAO_postprocess.process(m_gBuffer, scene.getCurrentCamera()->getProjection());

    /*
     * Lighting pass
     * */
    m_lighting_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader_lighting_pass.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(2));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_SSAO_postprocess.getOutTexture());

    scene.sendViewSpacePointLightDatas(m_shader_lighting_pass);
    scene.sendCameraToShader(m_shader_lighting_pass, m_width, m_height);

    m_quad.draw();

    m_HDR_postprocess.process(m_lighting_buffer);

    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT);
    m_quad_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_HDR_postprocess.getOutTexture());
    m_quad.draw();
}

void Renderer::reloadShaders()
{
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        m_shader_forward[i].reload();
        m_shader_geometry_pass[i].reload();
    }
}
