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
class Armature;
class Animation;
struct Channel;

class Model
{
public:
    Model();
    Model(Mesh *mesh, Material *material);
    virtual ~Model();

    virtual void draw(const Shader &shader, const GLfloat &render_time) const;

    void attachArmature(Armature *armature);
    void detachArmature();

    //  Getters
    inline GLuint getShaderTypeIndex() const{return m_material->getShaderTypeIndex();}
    GLboolean hasBone(const std::string &bone_name) const;
    Mesh *getMesh(const GLuint &index) const{return m_meshes[index];}
    Material *getMaterial() const{return m_material;}
    inline GLuint numberOfMeshes() const{return m_meshes.size();}

    //  Setters
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}
    inline void setMaterial(Material *material){m_material = material;}
    void setAnimationInfo(const std::string &animation_name, const GLuint &duration, const GLuint &ticks_per_sec);
    void setChannel(const std::string &animation_name, const std::string &bone_name, const GLuint &current_tick, const Channel &channel);

protected:
    std::vector<Mesh *> m_meshes;

    Material *m_material;
    ShaderType m_shader_type;

    glm::mat4 m_transform;

    Armature *m_armature;

    std::map<std::string, Animation> m_animations;
    Animation *m_current_animation;
};

#endif // MODEL_H
