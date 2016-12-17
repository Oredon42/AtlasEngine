#include "include/render/hdrpostprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

#include <QOpenGLFramebufferObject>

HDRPostProcess::HDRPostProcess(Quad &quad) :
    m_quad(quad),
    m_HDR(GL_TRUE),
    m_adaptation(GL_TRUE),
    m_bloom(GL_TRUE)
{

}

void HDRPostProcess::init(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;

    m_out_buffer.init(m_width, m_height);
    FramebufferTextureDatas out_textures_datas[2] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER),
                                                    FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER)};
    m_out_buffer.attachTextures(out_textures_datas, 2);


    /*
     * HDR
     * */
    m_hdr_shader.init("shaders/hdr.vert", "shaders/hdr.frag");
    m_hdr_shader.use();
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);
    m_hdr_buffer.init(m_width, m_height);
    FramebufferTextureDatas hdr_textures_datas[3] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER),
                                                    FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER),
                                                    FramebufferTextureDatas(GL_RGB16F, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER)};
    m_hdr_buffer.attachTextures(hdr_textures_datas, 3);

    /*
     * BLOOM
     * */
    m_blur_shaders[0].init("shaders/blur.vert", "shaders/blurv.frag");
    m_blur_shaders[1].init("shaders/blur.vert", "shaders/blurh.frag");
    m_blur_shaders[2].init("shaders/blur.vert", "shaders/blurmipmap.frag");
    m_blur_buffers[0].init(m_width, m_height);
    m_blur_buffers[1].init(m_width, m_height);
    FramebufferTextureDatas bloom_texture_datas[1] = {FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE)};
    m_blur_buffers[0].attachTextures(bloom_texture_datas, 1);
    m_blur_buffers[1].attachTextures(bloom_texture_datas, 1);

    /*
     * ADAPTATION
     * */
    m_downscaling_shader.init("shaders/downscaling.vert", "shaders/downscaling.frag");
    m_brightness_ping_buffer.init(64, 64);
    m_brightness_pong_buffer.init(16, 16);
    m_brightness_ping_buffer2.init(4, 4);
    m_brightness_pong_buffer2.init(1, 1);
    FramebufferTextureDatas adaptation_texture_datas[1] = {FramebufferTextureDatas(GL_RG16F, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR)};
    m_brightness_ping_buffer.attachTextures(adaptation_texture_datas, 1);
    m_brightness_pong_buffer.attachTextures(adaptation_texture_datas, 1);
    m_brightness_ping_buffer2.attachTextures(adaptation_texture_datas, 1);
    m_brightness_pong_buffer2.attachTextures(adaptation_texture_datas, 1);
    m_exposure_buffer.init(m_width, m_height);
    FramebufferTextureDatas exposure_texture_datas[1] = {FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR)};
    m_exposure_buffer.attachTextures(exposure_texture_datas, 1);
    m_exposure_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_FRAMEBUFFER, 0);
}

void HDRPostProcess::process()
{
    /*
     * Adaptation
     *
     * Ping-Pong between 2 FBO would need to reallocate at each loop
     * instead I use 2 more small FBO
     * */
    //  64x64 texture
    glViewport(0, 0, 64, 64);
    m_brightness_ping_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_downscaling_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(2));
    glUniform2f(glGetUniformLocation(m_downscaling_shader.getProgram(), "size"), m_hdr_buffer.width(), m_hdr_buffer.height());
    m_quad.draw();

    //  16x16 texture
    glViewport(0, 0, 16, 16);
    m_brightness_pong_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_downscaling_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightness_ping_buffer.getTexture(0));
    glUniform2f(glGetUniformLocation(m_downscaling_shader.getProgram(), "size"), 64.f, 64.f);
    m_quad.draw();

    //  4x4 texture
    glViewport(0, 0, 4, 4);
    m_brightness_ping_buffer2.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_downscaling_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightness_pong_buffer.getTexture(0));
    glUniform2f(glGetUniformLocation(m_hdr_shader.getProgram(), "size"), 16.f, 16.f);
    m_quad.draw();

    //  1x1 texture
    glViewport(0, 0, 1, 1);
    m_brightness_pong_buffer2.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_downscaling_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightness_ping_buffer2.getTexture(0));
    glUniform2f(glGetUniformLocation(m_hdr_shader.getProgram(), "size"), 4.f, 4.f);
    m_quad.draw();

    //  Get average & max luminances
    GLfloat average_luminance[2];
    glBindTexture(GL_TEXTURE_2D, m_brightness_pong_buffer2.getTexture(0));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, average_luminance);

    if(m_adaptation)
    {
        average_luminance[0] = fmax(0.05f, average_luminance[0]);
        average_luminance[1] = fmax(0.05f, average_luminance[1]);
    }

    /*
     * Bloom
     * */
    //  Get hdr 2nd texture + horizontal blur
    glViewport(0, 0, m_width/8, m_height/8);
    m_blur_shaders[2].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[0].getBuffer());
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(1));
    m_quad.draw();

    glViewport(0, 0, m_width, m_height);
    GLuint index_buffer = 0;
    GLuint i = 8;
    //  Ping-Pong
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
        glBlitFramebuffer(0, 0, m_width/(i*2), m_height/(i*2), 0, 0, m_width/i, m_height/i, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //  Horizontal blur
        m_blur_shaders[1].use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
        m_quad.draw();

        index_buffer = !index_buffer;
    }

    //  Last vertical blur
    m_blur_shaders[0].use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!index_buffer].getBuffer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[index_buffer].getTexture(0));
    m_quad.draw();


    /*
     * Compute out color + exposition
     * */
    glViewport(0, 0, m_width, m_height);
    m_out_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_hdr_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr_buffer.getTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[!index_buffer].getTexture(0));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_exposure_buffer.getTexture(0));

    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "hdr"), m_HDR);
    glUniform1i(glGetUniformLocation(m_hdr_shader.getProgram(), "bloom"), m_bloom);
    glUniform1f(glGetUniformLocation(m_hdr_shader.getProgram(), "L_avg"), average_luminance[0]);
    glUniform1f(glGetUniformLocation(m_hdr_shader.getProgram(), "L_max"), average_luminance[1]);
    m_quad.draw();

    //  Update old exposition
    m_out_buffer.bind(GL_READ_FRAMEBUFFER);
    m_exposure_buffer.bind(GL_DRAW_FRAMEBUFFER);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
