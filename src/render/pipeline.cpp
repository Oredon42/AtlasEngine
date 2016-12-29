#include "include/render/pipeline.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

Pipeline::Pipeline(const GLuint &width, const GLuint &height) :
    m_width(width),
    m_height(height)
{

}

Pipeline::~Pipeline()
{
    for(GLuint i = 0; i < m_processes.size(); ++i)
        delete m_processes[i];

    m_processes.clear();
}

void Pipeline::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const
{
    for(GLuint i = 0; i < m_processes.size(); ++i)
        m_processes[i]->process(quad, scene, render_time, keys);
}

void Pipeline::addProcess(RenderProcess *process)
{
    process->init(m_width, m_height);

    if(m_processes.size() > 0)
        process->connect(m_processes.back());
    else
        process->connect(0);

    m_processes.push_back(process);
}
