#ifndef PROCESS_H
#define PROCESS_H

#include "openglincludes.h"

class Quad;
class Framebuffer;
class Scene;

class RenderProcess
{
public:
    RenderProcess(const GLuint &width, const GLuint &height);
    virtual ~RenderProcess(){}

    inline virtual void resize(const GLuint &width, const GLuint &height){m_width = width; m_height = height;}

    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) = 0;

    inline void setPrevious(RenderProcess *process){m_previous_process = process;}
    inline void setFollowing(RenderProcess *process){m_following_process = process;}

    virtual void connect(RenderProcess *previous_process);

    //  Getters
    inline GLuint getOutTexture(const GLuint &i) const{return m_out_textures[i];}
    inline GLuint getNumTexture() const{return m_out_textures.size();}
    inline GLuint width() const{return m_width;}
    inline GLuint height() const{return m_height;}

    //  Setters
    inline virtual void setActivated(const GLboolean &activated){m_activated = activated;}

protected:
    std::string m_name;

    GLuint m_width;
    GLuint m_height;

    GLboolean m_activated;

    std::vector<GLuint> m_out_textures;

    RenderProcess *m_following_process;
    RenderProcess *m_previous_process;
};

#endif // PROCESS_H
