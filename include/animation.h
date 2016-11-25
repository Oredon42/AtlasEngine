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

    void setNumBones(const GLuint &n);
    void setDuration(const GLuint &d);
    void setChannel(const GLuint &bone_index, const GLuint &time_index, const Channel &channel);

//private:
    GLuint duration;
    glm::mat4 **transforms;
    GLuint num_bones;
    GLuint ticks_per_second;

    Channel **channels;
};

#endif // ANIMATION_H
