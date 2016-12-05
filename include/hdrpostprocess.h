#ifndef HDRPOSTPROCESS_H
#define HDRPOSTPROCESS_H

#include "openglincludes.h"

#include "shader.h"
#include "framebuffer.h"

class Quad;

class HDRPostProcess
{
public:
    HDRPostProcess(Quad &quad);

    void init(const GLuint &window_width, const GLuint &window_height);
    void process(const GLuint &width, const GLuint &height);

    //  Getters
    inline GLuint getHDRTexture()const {return m_hdr_buffer.getTexture(0);}
    inline GLuint getBlurTexture()const {return m_blur_buffers[1].getTexture(0);}

private:
    Quad &m_quad;

    Framebuffer m_hdr_buffer;
    Shader m_hdr_shader;
    GLuint m_color_buffer;
    GLfloat m_exposure;
    GLboolean m_hdr;
    GLboolean m_adaptation;

    //  BLOOM
    Framebuffer m_blur_buffers[2];
    Shader m_blur_shaders[3];
    GLboolean m_bloom;
};

#endif // HDRPOSTPROCESS_H
