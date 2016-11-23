#include "animation.h"

Animation::Animation() :
    duration(0),
    transforms(0),
    num_bones(0),
    ticks_per_second(0)
{

}

Animation::~Animation()
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

void Animation::setNumBones(const GLuint &n)
{
    if(transforms == 0)
    {
        num_bones = n;
        transforms = new glm::mat4*[num_bones];
        channels = new Channel*[num_bones];
    }
}

void Animation::setDuration(const GLuint &d)
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

void Animation::setChannel(const GLuint &bone_index, const GLuint &time_index, const Channel &channel)
{
    Channel &tmp=channels[bone_index][time_index];
    channels[bone_index][time_index] = channel;
}
