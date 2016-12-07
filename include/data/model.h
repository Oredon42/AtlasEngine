#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/quaternion.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#include "shader.h"
#include "mesh.h"
#include "material.h"

#define NUM_BONES_PER_VERTEX 4

class Camera;
class DirLight;
class SpotLight;
class PointLight;

class Model
{
public:
    Model();
    Model(Mesh *mesh, Material *material);
    ~Model();

    virtual void draw(const Shader &shader);

    //  Getters
    inline GLuint getShaderTypeIndex() const{return m_material->getShaderTypeIndex();}

    //  Setters
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}
    inline void setMaterial(const Material &material){m_material->copy(material);}

protected:
    std::vector<Mesh *> m_meshes;

    Material *m_material;
    ShaderType m_shader_type;

    glm::mat4 m_transform;
};

#endif // MODEL_H
