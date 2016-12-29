#include "include/render/process/renderprocess.h"
#include "include/data/quad.h"

RenderProcess::RenderProcess() :
    m_activated(GL_TRUE),
    m_previous_process(0)
{

}

void RenderProcess::init(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;
}

void RenderProcess::connect(RenderProcess *previous_process)
{
    m_previous_process = previous_process;
}
