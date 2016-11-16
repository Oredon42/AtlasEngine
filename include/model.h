#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>

#ifdef __APPLE__
    #include <QGLWidget>
#elif __linux__
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#elif _WIN32
    #include "lib/glew/glew.h"
#endif

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/quaternion.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "mesh.h"

#define NUM_BONES_PER_VERTEX 4

class Camera;
class DirLight;
class SpotLight;
class PointLight;

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

struct Material
{
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    Texture *texture;
};

class Model
{
public:
    Model();
    Model(Mesh *mesh, const std::vector<Texture> &textures, const ShaderType &shader_type, const GLboolean &has_normal_map);
    ~Model();

    virtual void draw(const Shader &shader);

    //  Getters
    inline GLuint getShaderTypeIndex() const{return m_shader_type.index;}

    //  Setters
    inline void setDiffuse(const glm::vec3 &diffuse){m_material.diffuse = diffuse;}
    inline void setSpecular(const glm::vec3 &specular){m_material.specular = specular;}
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}

protected:
    std::vector<Mesh *> m_meshes;

    std::vector<Texture> m_textures;

    Material m_material;
    ShaderType m_shader_type;
    GLboolean m_has_normal_map;

    glm::mat4 m_transform;
};

/******************
 * ANIMATED MODEL
 * *************** */

struct Bone
{
    GLuint parent;
    std::string name;
    std::vector<GLuint> children;

    glm::vec3 rotation;
    glm::mat4 bone_offset;
    glm::mat4 final_transformation;

    GLboolean used_in_animation;

    Bone()
    {
        parent = 0;
        bone_offset = glm::mat4();
        final_transformation = glm::mat4();
        used_in_animation = GL_FALSE;
    }
};

struct Channel
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;

    GLfloat time;

    void setVectors(const glm::vec3 &s, const glm::quat &r, const glm::vec3 p, const GLfloat &t)
    {
        scale = s;
        rotation = r;
        position = p;
        time = t;
    }
};

struct Animation
{
    GLuint duration;
    glm::mat4 **transforms;
    GLuint num_bones;
    GLuint ticks_per_second;

    Channel **channels;

    Animation() :
        duration(0),
        transforms(0),
        num_bones(0),
        ticks_per_second(0)
    {

    }

    ~Animation()
    {
        if(transforms != 0)
        {
            for(GLuint i = 0; i < num_bones; ++i)
            {
                delete[] transforms[i];
                delete[] channels[i];
            }
            delete[] transforms;
            delete[] channels;
        }
    }

    void setNumBones(const GLuint &n)
    {
        if(transforms == 0)
        {
            num_bones = n;
            transforms = new glm::mat4*[num_bones];
            channels = new Channel*[num_bones];
        }
    }

    void setDuration(const GLuint &d)
    {
        if(duration == 0)
        {
            duration = d;
            for(GLuint i = 0; i < num_bones; ++i)
            {
                transforms[i] = new glm::mat4[duration];
                channels[i] = new Channel[duration];
            }
        }
    }

    void setTransforms(const GLuint &bone_index, const GLuint &time_index, const GLfloat &time, const glm::vec3 &scale, const glm::quat &rotation, const glm::vec3 &position)
    {
        channels[bone_index][time_index].setVectors(scale, rotation, position, time);
    }
};

class AnimatedModel : public Model
{
    static const GLuint MAX_BONES = 100;
public:
    AnimatedModel(AnimatedMesh *mesh, const std::vector<Texture> &textures, const ShaderType &shader_type, const std::map<std::string, GLuint> &bone_mapping, const GLuint &num_bones, Bone *&armature, const GLboolean &has_normal_map, GLfloat &render_time);

    virtual void draw(const Shader &shader);

    //  Getters
    inline GLboolean hasBone(const std::string &bone_name){return m_bone_mapping.find(bone_name) != m_bone_mapping.end();}

    //  Setters
    void setAnimationInfo(const std::string &animation_name, const GLuint &num_bones, const GLuint &duration, const GLuint &ticks_per_sec, const std::string &bone_name, const GLuint &current_tick, const GLfloat &current_time, const glm::vec3 &scaling, const glm::quat &rotation, const glm::vec3 &position);
    void setBoneParent(const std::string &parent_name, const std::string &child_name);
    void setAnimation(const std::string &current_animation);

protected:
    glm::vec3 CalcInterpolatedPosition(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const;
    glm::quat CalcInterpolatedRotation(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const;
    glm::vec3 CalcInterpolatedScaling(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const;
    void updateArmature(const Animation &anim, Bone &current_bone, const GLuint &bone_index, const glm::mat4 &parent_transform, const GLfloat &render_time);

    GLuint m_bone_buffer;

    //  Bones
    Bone *m_armature;
    GLuint m_num_bones;
    std::map<std::string, GLuint> m_bone_mapping;

    glm::mat4 m_global_inverse_transform;

    //  Animations
    std::string m_current_animation;
    std::map<std::string, Animation> m_animations;


    GLuint m_bone_location[MAX_BONES];

    GLfloat &m_render_time;
};

#endif // MODEL_H
