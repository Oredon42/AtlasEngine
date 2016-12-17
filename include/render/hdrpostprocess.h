#ifndef HDRPOSTPROCESS_H
#define HDRPOSTPROCESS_H

#include "openglincludes.h"

#include "shader.h"
#include "framebuffer.h"

class Scene;
class Quad;

class HDRPostProcess
{
public:
    HDRPostProcess(Quad &quad);

    void init(const GLuint &width, const GLuint &height);
    void process();

    inline void bindBuffer()const {m_hdr_buffer.bind();}

    //  Getters
    inline GLuint getHDRTexture()const {return m_hdr_buffer.getTexture(0);}
    inline GLuint getBlurTexture()const {return m_blur_buffers[1].getTexture(0);}
    inline GLuint getOutTexture()const {return m_out_buffer.getTexture(0);}

    //  Setters
    inline void switchHDR(){m_HDR = !m_HDR;}
    inline void switchAdaptation(){m_adaptation = !m_adaptation;}
    inline void switchBloom(){m_bloom = !m_bloom;}

private:
    Quad &m_quad;

    GLuint m_width;
    GLuint m_height;

    Framebuffer m_hdr_buffer;
    Shader m_hdr_shader;
    GLuint m_color_buffer;
    GLboolean m_HDR;
    GLboolean m_adaptation;

    //  BLOOM
    Framebuffer m_blur_buffers[2];
    Shader m_blur_shaders[3];
    GLboolean m_bloom;

    //ADAPTATION
    Framebuffer m_brightness_ping_buffer;
    Framebuffer m_brightness_ping_buffer2;
    Framebuffer m_brightness_pong_buffer;
    Framebuffer m_brightness_pong_buffer2;
    Shader m_downscaling_shader;
    Framebuffer m_exposure_buffer;

    Framebuffer m_out_buffer;
};

#endif // HDRPOSTPROCESS_H
