#include "include/render/process/renderprocess.h"
#include "include/data/quad.h"

RenderProcess::RenderProcess(const GLuint &width, const GLuint &height) :
    m_width(width),
    m_height(height),
    m_activated(GL_TRUE),
    m_previous_process(0),
    m_following_process(0)
{

}

void RenderProcess::connect(RenderProcess *previous_process)
{
    m_previous_process = previous_process;
    if(previous_process != 0)
        previous_process->m_following_process = this;
}
