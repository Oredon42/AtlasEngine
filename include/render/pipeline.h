#ifndef PIPELINE_H
#define PIPELINE_H

#include <openglincludes.h>
#include <vector>

#include "include/render/process/renderprocess.h"

class Quad;
class Scene;

class Pipeline
{
public:
    Pipeline(const GLuint &width, const GLuint &height);
    ~Pipeline();

    void init(const GLuint &width, const GLuint &height);

    void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const;

    void addProcess(RenderProcess *process);

    //  Getter
    inline GLuint getOutTexture() const{return m_processes.back()->getOutTexture(0);}
    std::vector<MenuElement *> getMenuElements() const{return m_menu_elements;}

private:
    GLuint m_width;
    GLuint m_height;

    std::vector<RenderProcess *> m_processes;
    std::vector<MenuElement *> m_menu_elements;
};

#endif // PIPELINE_H
