#ifndef PROCESS_H
#define PROCESS_H

#include "openglincludes.h"
#include "menuelement.h"
#include "texture.h"

#include <initializer_list>

class Quad;
class Framebuffer;
class Scene;

class RenderProcess
{
    /*
     * Link between 2 processes in order to send out
     * texture of a process to another as inputs
     * the link is between a IN and a OUT process
     * IN process output textures are linked to OUT
     * process inputs textures
     * */
    struct RenderProcessLink
    {
        RenderProcess *in_render_process_pointer;   //  Pointer to IN process
        GLchar in_texture_index;                    //  Index of the IN texture in the IN process textures array

        RenderProcess *out_render_process_pointer;  //  Pointer to OUT process
        GLchar out_texture_index;                   //  Index of the OUT texture in the OUT process textures array
    };

public:
    //  CLASS METHOD
    static GLboolean linkProcesses(RenderProcess *r1, RenderProcess *r2, const std::initializer_list<GLchar> &r1_texture_indices, const std::initializer_list<GLchar> &r2_texture_indices);


    // METHODS

    RenderProcess(const GLchar &num_inputs, const GLchar &num_outputs);
    virtual ~RenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement(){}

    inline virtual void resize(const GLuint &width, const GLuint &height){m_width = width; m_height = height;}
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) = 0;

    //  Getters
    inline Texture *getOutTexture(const GLuint &i) const{return m_out_textures[i];}
    inline GLuint getNumOutTexture() const{return m_out_textures.size();}
    inline GLuint width() const{return m_width;}
    inline GLuint height() const{return m_height;}
    inline MenuElement *getMenuElement() const{return m_menu_element;}
    inline GLchar numberIn() const{return m_in_links.size();}
    inline GLchar numberOut() const{return m_out_links.size();}
    inline RenderProcess *getInputProcess(const GLuint &i) const{return m_in_links[i]->in_render_process_pointer;}
    inline GLboolean inLinkIsNull(const GLchar &index) const{return m_in_links[index] == NULL;}

    //  Setters
    inline virtual void setActivated(const GLboolean &activated){m_activated = activated;}
    inline void setInLink(RenderProcessLink *link, const GLchar &link_index){m_in_links[link_index] = link;}
    inline void addOutLink(RenderProcessLink *link, const GLchar &link_index){m_out_links[link_index].push_back(link);}

protected:
    inline void bindPreviousTexture(const GLuint &index) const{m_in_links[index]->in_render_process_pointer->getOutTexture(m_in_links[index]->in_texture_index)->bind();}

    GLuint m_width;
    GLuint m_height;

    GLboolean m_activated;

    std::vector<RenderProcessLink *> m_in_links;                //  A process receive 1 input for each texture
    std::vector<std::vector<RenderProcessLink *> > m_out_links; //  A process can send its outputs to several processes
    std::vector<Texture *> m_out_textures;

    MenuElement *m_menu_element;
};

#endif // PROCESS_H
