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
    void resize(const GLuint &width, const GLuint &height);

    void process(const Framebuffer &lighting_buffer);

    //  Getters
    inline GLuint getOutTexture()const {return m_out_texture;}

    //  Setters
    void setActivated(const GLboolean &activated);
    inline void switchHDR(){m_HDR = !m_HDR;}
    inline void switchAdaptation(){m_adaptation = !m_adaptation;}
    inline void switchBloom(){m_bloom = !m_bloom;}

private:
    void processAdaptation(const Framebuffer &lighting_buffer);
    void processBloom(const Framebuffer &lighting_buffer);

    GLuint m_width;
    GLuint m_height;

    Quad &m_quad;

    GLboolean m_activated;

    GLuint m_out_texture;

    Shader m_HDR_shader;
    GLboolean m_HDR;
    GLboolean m_adaptation;

    //  BLOOM
    Framebuffer m_blur_buffers[2];
    Shader m_blur_shaders[3];
    GLboolean m_bloom;
    GLuint m_bloom_quality;
    GLuint m_buffer_index;

    //ADAPTATION
    Framebuffer m_brightness_ping_buffer;
    Framebuffer m_brightness_ping_buffer2;
    Framebuffer m_brightness_pong_buffer;
    Framebuffer m_brightness_pong_buffer2;
    Shader m_downscaling_shader;
    Framebuffer m_exposure_buffer;
    GLfloat m_avg_max_luminances[2];

    Framebuffer m_out_buffer;
};

#endif // HDRPOSTPROCESS_H
