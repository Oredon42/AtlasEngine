#include "include/render/process/renderprocess.h"
#include "include/data/quad.h"

RenderProcess::RenderProcess(const GLchar &num_inputs, const GLchar &num_outputs) :
    m_activated(GL_TRUE),
    m_in_links(num_inputs),
    m_out_links(num_outputs),
    m_out_textures(num_outputs),
    m_menu_element(0)
{

}

RenderProcess::~RenderProcess()
{
    for(size_t i = 0; i < m_in_links.size(); ++i)
        delete m_in_links[i];
}

void RenderProcess::init(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;

    initMenuElement();
}

GLboolean RenderProcess::linkProcesses(RenderProcess *r1, RenderProcess *r2, const std::initializer_list<GLchar> &r1_texture_indices, const std::initializer_list<GLchar> &r2_texture_indices)
{
    if(r1_texture_indices.size() > 0 && r1_texture_indices.size() == r2_texture_indices.size())
    {
        for(const GLchar *texture_index_r1 = r1_texture_indices.begin(), *texture_index_r2 = r2_texture_indices.begin(); texture_index_r1 != r1_texture_indices.end(); ++texture_index_r1, ++texture_index_r2)
        {
            if(r2->inLinkIsNull(*texture_index_r2))
            {
                RenderProcessLink *link = new RenderProcessLink();
                link->in_render_process_pointer = r1;
                link->in_texture_index = *texture_index_r1;
                link->out_render_process_pointer = r2;
                link->out_texture_index = *texture_index_r2;
                r1->addOutLink(link, *texture_index_r1);
                r2->setInLink(link, *texture_index_r2);
            }
            else
                std::cerr << "Warning: process has already an input for this texture" << std::endl;
        }

        return GL_TRUE;
    }

    return GL_FALSE;
}
