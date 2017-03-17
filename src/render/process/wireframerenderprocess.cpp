#include "include/render/process/wireframerenderprocess.h"
#include "include/data/quad.h"
#include "include/data/scene.h"

WireframeRenderProcess::WireframeRenderProcess() :
    RenderProcess(1)
{

}

void WireframeRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(width, height);

    m_color_shader.init("shaders/color.vert", "shaders/color.frag");

    std::vector<FramebufferTextureDatas> texture_datas;
    texture_datas.push_back(FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT));

    std::vector<FramebufferRenderbufferDatas> renderbuffer_datas;
    renderbuffer_datas.push_back(FramebufferRenderbufferDatas(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT));

    m_out_buffer.init(width, height);
    m_out_buffer.attachTextures(texture_datas);
    m_out_buffer.attachRenderBuffers(renderbuffer_datas);

    m_out_textures.push_back(m_out_buffer.getTexture(0));
}

void WireframeRenderProcess::initMenuElement()
{

}

void WireframeRenderProcess::resize(const GLuint &width, const GLuint &height)
{
    /*RenderProcess::resize(width, height);

    m_SSAO_buffer.resize(m_width, m_height);
    m_SSAO_blur_buffer.resize(m_width, m_height);*/
}

void WireframeRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_out_buffer.bind();
    glViewport(0, 0, m_out_buffer.width(), m_out_buffer.height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_color_shader.use();
    glUniform3f(glGetUniformLocation(m_color_shader.getProgram(), "color"), 0.f, 0.f, 0.f);
    scene.draw(m_color_shader, keys, render_time, m_width, m_height);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.f, 1.f);
    m_color_shader.use();
    glUniform3f(glGetUniformLocation(m_color_shader.getProgram(), "color"), 1.f, 1.f, 1.f);
    scene.draw(m_color_shader, keys, render_time, m_width, m_height);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WireframeRenderProcess::setActivated(const GLboolean &activated)
{
    m_activated = activated;
}
