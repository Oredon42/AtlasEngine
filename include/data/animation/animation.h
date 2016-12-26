#ifndef ANIMATION_H
#define ANIMATION_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

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

class Animation
{
public:
    Animation();
    ~Animation();

    glm::vec3 calcInterpolatedPosition(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const;
    glm::quat calcInterpolatedRotation(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const;
    glm::vec3 calcInterpolatedScaling(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const;

    //  Getters
    inline Channel &getChannel(const GLuint &bone_index, const GLuint &time_index) const{return m_channels[bone_index][time_index];}
    inline GLuint numberOfBones() const{return m_num_bones;}
    inline GLuint getTicksPerSecond() const{return m_ticks_per_second;}
    inline GLuint getDuration() const{return m_duration;}

    //  Setters
    void setNumBones(const GLuint &n);
    void setDuration(const GLuint &d);
    void setChannel(const GLuint &bone_index, const GLuint &time_index, const Channel &channel);
    void setTicksPerSecond(const GLuint &ticks_per_second){m_ticks_per_second = ticks_per_second;}

private:
    GLuint m_duration;
    glm::mat4 **m_transforms;
    GLuint m_num_bones;
    GLuint m_ticks_per_second;

    Channel **m_channels;
};

#endif // ANIMATION_H
