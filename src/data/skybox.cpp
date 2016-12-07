#include "include/data/skybox.h"

Skybox::Skybox() :
m_initialised(false)
{

}

/*Skybox::Skybox(std::string texture_path, std::string path) :
Model("/obj/skybox/skybox.obj", path, "/shaders/skybox.vert", "/shaders/skybox.frag"),
m_initialised(true)
{
    Model::addEnvMap(texture_path);
}*/

/*void Skybox::init(std::string texture_path, std::string path)
{
    if(!m_initialised)
    {
        Model::init("/obj/skybox/skybox.obj", path);
        m_shader.init("/shaders/skybox.vert", "/shaders/skybox.frag", path);
        Model::addEnvMap(texture_path);
    }
    m_initialised = true;
}*/

void Skybox::Draw(glm::mat4 view)
{
    glDepthMask(GL_FALSE);
    //glUniformMatrix4fv(glGetUniformLocation(m_shader.m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    //Model::Draw();

    glDepthMask(GL_TRUE);
}

GLboolean Skybox::isInitialised() const
{
    return m_initialised;
}
