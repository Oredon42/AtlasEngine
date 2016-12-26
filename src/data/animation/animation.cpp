#include "include/data/animation/animation.h"

Animation::Animation() :
    m_duration(0),
    m_transforms(0),
    m_num_bones(0),
    m_ticks_per_second(0)
{

}

Animation::~Animation()
{
    if(m_transforms != 0)
    {
        for(GLuint i = 0; i < m_num_bones; ++i)
        {
            delete[] m_transforms[i];
            delete[] m_channels[i];
        }
        delete[] m_transforms;
        delete[] m_channels;
    }
}

void Animation::setNumBones(const GLuint &n)
{
    if(m_transforms == 0)
    {
        m_num_bones = n;
        m_transforms = new glm::mat4*[m_num_bones];
        m_channels = new Channel*[m_num_bones];
    }
}

void Animation::setDuration(const GLuint &d)
{
    if(m_duration == 0)
    {
        m_duration = d;
        for(GLuint i = 0; i < m_num_bones; ++i)
        {
            m_transforms[i] = new glm::mat4[m_duration];
            m_channels[i] = new Channel[m_duration];
        }
    }
}

void Animation::setChannel(const GLuint &bone_index, const GLuint &time_index, const Channel &channel)
{
    m_channels[bone_index][time_index] = channel;
}

/*
 *  Calculates the interpolation of the position
 *  at the time animation_time
 * */
glm::vec3 Animation::calcInterpolatedPosition(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const
{
    if(animation_duration == 1)
        return channel1.position;

    glm::vec3 delta = channel2.position - channel1.position;
    return channel1.position + factor * delta;
}


/*
 *  Calculates the interpolation of the rotation
 *  at the time animation_time
 * */
glm::quat Animation::calcInterpolatedRotation(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const
{
    if(animation_duration == 1)
        return channel1.rotation;

    return glm::normalize(glm::slerp(channel1.rotation, channel2.rotation, factor));
}


/*
 *  Calculates the interpolation of the scale
 *  at the time animation_time
 * */
glm::vec3 Animation::calcInterpolatedScaling(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const
{
    if(animation_duration == 1)
        return channel1.scale;

    glm::vec3 delta = channel2.scale - channel1.scale;
    return channel1.scale + factor * delta;
}
