#ifndef PIPELINE_H
#define PIPELINE_H

#include <openglincludes.h>
#include <vector>

#include "include/render/process/geometryrenderprocess.h"
#include "include/render/process/lightingrenderprocess.h"

class RenderProcess;
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
    inline GLuint getOutTexture() const{return m_processes.back()->getOutTexture(0);}

private:
    GLuint m_width;
    GLuint m_height;

    std::vector<RenderProcess *> m_processes;
};

#endif // PIPELINE_H
