#include "include/data/model.h"
#include "include/data/camera.h"
#include "include/data/dirlight.h"
#include "include/data/spotlight.h"
#include "include/data/pointlight.h"

Model::Model()
{

}

Model::Model(Mesh *mesh, Material *material) :
    m_material(material)
{
    m_meshes.push_back(mesh);
}

Model::~Model()
{
    for(GLuint i = 0; i < m_meshes.size(); ++i)
        delete m_meshes[i];

    m_meshes.clear();

    delete m_material;
}

void Model::draw(const Shader &shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(m_transform));

    m_material->sendDatas(shader);

    for(GLuint i = 0; i < m_meshes.size(); ++i)
        m_meshes[i]->draw();
}
