#include "include/render/process/hdrrenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

#include <QOpenGLFramebufferObject>
#include <QtWidgets>

HDRRenderProcess::HDRRenderProcess() :
    m_HDR(GL_FALSE),
    m_adaptation(GL_FALSE),
    m_bloom(GL_FALSE),
    m_chromatic_aberration(GL_FALSE),
    m_bloom_quality(8),
    m_buffer_index(0)
{

}

void HDRRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(width, height);

    m_out_buffer.init(m_width, m_height);
    std::vector<FramebufferTextureDatas> out_textures_datas;
    out_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    out_textures_datas.push_back(FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER));
    m_out_buffer.attachTextures(out_textures_datas);

    /*
     * HDR
     * */
    m_HDR_shader.initDefine("hdr", Define("#define HDR", m_HDR));
    m_HDR_shader.initDefine("bloom", Define("#define BLOOM", m_bloom));
    m_HDR_shader.initDefine("adaptation", Define("#define ADAPTATION", m_adaptation));
    m_HDR_shader.initDefine("chromatic_aberration", Define("#define CHROMATIC_ABERRATION", m_chromatic_aberration));
    m_HDR_shader.init("shaders/hdr.vert", "shaders/hdr.frag");
    m_HDR_shader.use();
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);

    /*
     * BLOOM
     * */
    m_blur_shaders[0].init("shaders/blur.vert", "shaders/blurv.frag");
    m_blur_shaders[1].init("shaders/blur.vert", "shaders/blurh.frag");
    m_blur_shaders[2].init("shaders/blur.vert", "shaders/blurmipmap.frag");
    m_blur_buffers[0].init(m_width, m_height);
    m_blur_buffers[1].init(m_width, m_height);
    std::vector<FramebufferTextureDatas> bloom_texture_datas;
    bloom_texture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE));
    m_blur_buffers[0].attachTextures(bloom_texture_datas);
    m_blur_buffers[1].attachTextures(bloom_texture_datas);

    /*
     * ADAPTATION
     * */
    m_downscaling_shader.init("shaders/downscaling.vert", "shaders/downscaling.frag");
    m_brightness_ping_buffer.init(64, 64);
    m_brightness_pong_buffer.init(16, 16);
    m_brightness_ping_buffer2.init(4, 4);
    m_brightness_pong_buffer2.init(1, 1);
    std::vector<FramebufferTextureDatas> adaptation_texture_datas;
    adaptation_texture_datas.push_back(FramebufferTextureDatas(GL_RG16F, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR));
    m_brightness_ping_buffer.attachTextures(adaptation_texture_datas);
    m_brightness_pong_buffer.attachTextures(adaptation_texture_datas);
    m_brightness_ping_buffer2.attachTextures(adaptation_texture_datas);
    m_brightness_pong_buffer2.attachTextures(adaptation_texture_datas);
    m_exposure_buffer.init(m_width, m_height);
    std::vector<FramebufferTextureDatas> exposure_texture_datas;
    exposure_texture_datas.push_back(FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR));
    m_exposure_buffer.attachTextures(exposure_texture_datas);
    m_exposure_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_FRAMEBUFFER, 0);

    m_out_textures.push_back(m_out_buffer.getTexture(0));

    m_avg_lum_location = glGetUniformLocation(m_HDR_shader.getProgram(), "L_avg");
    m_max_lum_location = glGetUniformLocation(m_HDR_shader.getProgram(), "L_max");
}

void HDRRenderProcess::initMenuElement()
{
    QLabel *HDR_label = new QLabel("HDR");
    QLabel *bloom_label = new QLabel("Bloom");
    QLabel *adaptation_label = new QLabel("Eye adaptation");
    QLabel *chromatic_aberration_label = new QLabel("Chromatic Aberration");

    QCheckBox *HDR_checkbox = new QCheckBox;
    QCheckBox *bloom_checkbox = new QCheckBox;
    QCheckBox *adaptation_checkbox = new QCheckBox;
    QCheckBox *chromatic_aberration_checkbox = new QCheckBox;

    MenuElement::connect(HDR_checkbox, SIGNAL(clicked()), this, SLOT(switchHDR()));
    MenuElement::connect(bloom_checkbox, SIGNAL(clicked()), this, SLOT(switchbloom()));
    MenuElement::connect(adaptation_checkbox, SIGNAL(clicked()), this, SLOT(switchAdaptation()));
    MenuElement::connect(chromatic_aberration_checkbox, SIGNAL(clicked()), this, SLOT(switchChromaticAberration()));

    QHBoxLayout *HDR_horizontal_layout = new QHBoxLayout;
    QHBoxLayout *bloom_horizontal_layout = new QHBoxLayout;
    QHBoxLayout *adaptation_horizontal_layout = new QHBoxLayout;
    QHBoxLayout *chromatic_aberration_layout = new QHBoxLayout;

    HDR_horizontal_layout->addWidget(HDR_label);
    HDR_horizontal_layout->addWidget(HDR_checkbox);

    bloom_horizontal_layout->addWidget(bloom_label);
    bloom_horizontal_layout->addWidget(bloom_checkbox);

    adaptation_horizontal_layout->addWidget(adaptation_label);
    adaptation_horizontal_layout->addWidget(adaptation_checkbox);

    chromatic_aberration_layout->addWidget(chromatic_aberration_label);
    chromatic_aberration_layout->addWidget(chromatic_aberration_checkbox);

    QWidget *HDR_widget = new QWidget;
    QWidget *bloom_widget = new QWidget;
    QWidget *adaptation_widget = new QWidget;
    QWidget *chromatic_aberration_widget = new QWidget;

    HDR_widget->setLayout(HDR_horizontal_layout);
    bloom_widget->setLayout(bloom_horizontal_layout);
    adaptation_widget->setLayout(adaptation_horizontal_layout);
    chromatic_aberration_widget->setLayout(chromatic_aberration_layout);

    QVBoxLayout *vertical_layout = new QVBoxLayout;
    vertical_layout->addWidget(HDR_widget);
    vertical_layout->addWidget(bloom_widget);
    vertical_layout->addWidget(adaptation_widget);
    vertical_layout->addWidget(chromatic_aberration_widget);

    m_menu_element->setLayout(vertical_layout);
}

void HDRRenderProcess::resize(const GLuint &width, const GLuint &height)
{
    RenderProcess::resize(width, height);

    m_blur_buffers[0].resize(m_width, m_height);
    m_blur_buffers[1].resize(m_width, m_height);
    m_brightness_ping_buffer.resize(m_width, m_height);
    m_brightness_ping_buffer2.resize(m_width, m_height);
    m_brightness_pong_buffer.resize(m_width, m_height);
    m_brightness_pong_buffer2.resize(m_width, m_height);
    m_exposure_buffer.resize(m_width, m_height);
    m_out_buffer.resize(m_width, m_height);
}

void HDRRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    processAdaptation(quad);
    processBloom(quad);
    
    // Compute out color + exposition
    glViewport(0, 0, m_out_buffer.width(), m_out_buffer.height());
    m_out_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_HDR_shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[!m_buffer_index].getTexture(0));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_exposure_buffer.getTexture(0));
    
    glUniform1f(m_max_lum_location, m_avg_max_luminances[0]);
    glUniform1f(m_avg_lum_location, m_avg_max_luminances[1]);

    quad.draw();
    
    //  Update old exposition
    m_out_buffer.bind(GL_READ_FRAMEBUFFER);
    m_exposure_buffer.bind(GL_DRAW_FRAMEBUFFER);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, m_out_buffer.width(), m_out_buffer.height(), 0, 0, m_exposure_buffer.width(), m_exposure_buffer.height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void HDRRenderProcess::processAdaptation(const Quad &quad)
{
    /*
     * Ping-Pong between 2 FBO would need to reallocate at each loop
     * instead I use 4 FBO
     * */
    //  64x64 texture
    glViewport(0, 0, 64, 64);
    m_brightness_ping_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_downscaling_shader.use();

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(2));
    glUniform2f(glGetUniformLocation(m_downscaling_shader.getProgram(), "size"), m_previous_process->width(), m_previous_process->height());
    quad.draw();


    //  16x16 texture
    glViewport(0, 0, 16, 16);
    m_brightness_pong_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_downscaling_shader.use();

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_brightness_ping_buffer.getTexture(0));
    glUniform2f(glGetUniformLocation(m_downscaling_shader.getProgram(), "size"), 64.f, 64.f);
    quad.draw();


    //  4x4 texture
    glViewport(0, 0, 4, 4);
    m_brightness_ping_buffer2.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_downscaling_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightness_pong_buffer.getTexture(0));
    glUniform2f(glGetUniformLocation(m_HDR_shader.getProgram(), "size"), 16.f, 16.f);
    quad.draw();

    //  1x1 texture
    glViewport(0, 0, 1, 1);
    m_brightness_pong_buffer2.bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_downscaling_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightness_ping_buffer2.getTexture(0));

    glUniform2f(glGetUniformLocation(m_HDR_shader.getProgram(), "size"), 4.f, 4.f);
    quad.draw();

    //  Get average & max luminances
    glBindTexture(GL_TEXTURE_2D, m_brightness_pong_buffer2.getTexture(0));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, m_avg_max_luminances);

    if(m_adaptation)
    {
        m_avg_max_luminances[0] = fmax(0.05f, m_avg_max_luminances[0]);
        m_avg_max_luminances[1] = fmax(0.05f, m_avg_max_luminances[1]);
    }
    else
    {
        m_avg_max_luminances[0] = 1.f;
        m_avg_max_luminances[1] = 1.f;
    }
}

void HDRRenderProcess::processBloom(const Quad &quad)
{
    //  Get hdr 2nd texture + horizontal blur
    glViewport(0, 0, m_width/8, m_height/8);
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[0].getBuffer());
    glClear(GL_COLOR_BUFFER_BIT);

    m_blur_shaders[2].use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_previous_process->getOutTexture(1));
    quad.draw();

    GLuint i = m_bloom_quality;
    m_buffer_index = 0;
    glViewport(0, 0, m_width, m_height);
    //  Ping-Pong
    while(i > 1)
    {
        //  Vertical blur
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!m_buffer_index].getBuffer());

        m_blur_shaders[0].use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[m_buffer_index].getTexture(0));
        quad.draw();

        i>>=1;

        //  Bilinear interpolation to size*2
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_blur_buffers[!m_buffer_index].getBuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_blur_buffers[m_buffer_index].getBuffer());
        glBlitFramebuffer(0, 0, m_width/(i*2), m_height/(i*2), 0, 0, m_width/i, m_height/i, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //  Horizontal blur
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!m_buffer_index].getBuffer());

        m_blur_shaders[1].use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffers[m_buffer_index].getTexture(0));
        quad.draw();

        m_buffer_index = !m_buffer_index;
    }

    //  Last vertical blur
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur_buffers[!m_buffer_index].getBuffer());

    m_blur_shaders[0].use();
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_blur_buffers[m_buffer_index].getTexture(0));
    quad.draw();
}

void HDRRenderProcess::setActivated(const GLboolean &activated)
{
    RenderProcess::setActivated(activated);

    //m_out_texture = m_out_buffer.getTexture(0);
}


void HDRRenderProcess::switchHDR()
{
    m_HDR = !m_HDR;
    m_HDR_shader.setDefined("hdr", m_HDR);
    m_HDR_shader.reload();
}

void HDRRenderProcess::switchbloom()
{
    m_bloom = !m_bloom;
    m_HDR_shader.setDefined("bloom", m_bloom);
    m_HDR_shader.reload();

    m_HDR_shader.use();
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);
}

void HDRRenderProcess::switchAdaptation()
{
    m_adaptation = !m_adaptation;
    m_HDR_shader.setDefined("adaptation", m_adaptation);
    m_HDR_shader.reload();

    m_HDR_shader.use();
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);
}

void HDRRenderProcess::switchChromaticAberration()
{
    m_chromatic_aberration = !m_chromatic_aberration;
    m_HDR_shader.setDefined("chromatic_aberration", m_chromatic_aberration);
    m_HDR_shader.reload();

    m_HDR_shader.use();
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "scene"), 0);
    glUniform1i(glGetUniformLocation(m_HDR_shader.getProgram(), "bloomBlur"), 1);
    glUseProgram(0);
}