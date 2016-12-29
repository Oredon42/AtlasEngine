#ifndef HDRPROCESS_H
#define HDRPROCESS_H

#include "openglincludes.h"

#include "renderprocess.h"
#include "shader.h"
#include "framebuffer.h"

class Scene;
class Quad;

class HDRRenderProcess : public RenderProcess
{
public:
    HDRRenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);

    virtual void resize(const GLuint &width, const GLuint &height);

    void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Setters
    virtual void setActivated(const GLboolean &activated);
    inline void switchHDR(){m_HDR = !m_HDR;}
    inline void switchAdaptation(){m_adaptation = !m_adaptation;}
    inline void switchBloom(){m_bloom = !m_bloom;}

private:
    void processAdaptation(const Quad &quad);
    void processBloom(const Quad &quad);

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

    GLint m_HDR_location;
    GLint m_bloom_location;
    GLint m_avg_lum_location;
    GLint m_max_lum_location;
};

#endif // HDRPROCESS_H
