#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H

#include "include/data/model.h"
#include "include/data/animatedmesh.h"

class Animation;
struct Channel;

struct Bone
{
    GLuint parent;
    std::string name;
    std::vector<GLuint> children;

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

class AnimatedModel : public Model
{
    static const GLuint MAX_BONES = 100;
public:
    AnimatedModel(AnimatedMesh *mesh, const std::map<std::string, GLuint> &bone_mapping, const GLuint &num_bones, Bone *&armature, GLfloat &render_time, Material *material);

    virtual void draw(const Shader &shader);

    void buildBoneTree(const aiNode *ai_node);

    //  Getters
    inline GLboolean hasBone(const std::string &bone_name){return m_bone_mapping.find(bone_name) != m_bone_mapping.end();}

    //  Setters
    void setAnimationInfo(const std::string &animation_name, const GLuint &duration, const GLuint &ticks_per_sec);
    void setChannel(const std::string &animation_name, const std::string &bone_name, const GLuint &current_tick, const Channel &channel);
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

#endif // ANIMATEDMODEL_H
