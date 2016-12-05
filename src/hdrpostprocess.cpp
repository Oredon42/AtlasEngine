#include "hdrpostprocess.h"
#include "include/quad.h"

HDRPostProcess::HDRPostProcess(Quad &quad) :
    m_quad(quad)
{

}

void HDRPostProcess::init(const GLuint &window_width, const GLuint &window_height)
{
    /*
     * HDR
     * */
    m_hdr_shader.init("shaders/hdr.vert", "shaders/hdr.frag");
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
    m_blur_shaders[0].init("shaders/blur.vert", "shaders/blurv.frag");
    m_blur_shaders[1].init("shaders/blur.vert", "shaders/blurh.frag");
    m_blur_shaders[2].init("shaders/blur.vert", "shaders/blurmipmap.frag");
    m_blur_buffers[0].init(window_width, window_height);
    m_blur_buffers[1].init(window_width, window_height);
    FramebufferTextureDatas bloom_texture_datas[1] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT)};
    m_blur_buffers[0].attachTextures(bloom_texture_datas, 1, GL_CLAMP_TO_EDGE);
    m_blur_buffers[1].attachTextures(bloom_texture_datas, 1, GL_CLAMP_TO_EDGE);
}

void HDRPostProcess::process(const GLuint &width, const GLuint &height)
{
    /*
     * Adaptation
     * */
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(2));
    glGenerateMipmap(GL_TEXTURE_2D);

    //  Use last level of mipmap to get the average brightness of the scene
    if(m_adaptation)
    {
        GLuint num_levels = floor(log2(std::max(width, height)));
        GLfloat average[1];
        glGetTexImage(GL_TEXTURE_2D, num_levels, GL_RED, GL_FLOAT, average);

        GLfloat average_brightness = glm::min(glm::max(average[0], 0.05f), 1.f);

        GLfloat target_exposure = 0.5f / average_brightness;
        m_exposure = m_exposure + (target_exposure - m_exposure) * 0.1f;
    }

    //  Get hdr 2nd texture + horizontal blur
    glViewport(0, 0, width/8, height/8);
    m_blur_shaders[2].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[0].getBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(1));
    m_quad.draw();

    glViewport(0, 0, width, height);
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

        i>>=1;

        //  Bilinear interpolation to size*2
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_blur_buffers[index_buffer].getBuffer());
        glBlitFramebuffer(0, 0, width/(i*2), height/(i*2), 0, 0, width/i, height/i, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //  Horizontal blur
        m_blur_shaders[1].use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
        m_quad.draw();

        index_buffer = !index_buffer;
    }

    //  Vertical
    m_blur_shaders[0].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
    m_quad.draw();
}
