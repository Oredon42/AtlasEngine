#include "core/animation.h"

//  Channel
Channel::Channel()
{

}

Channel::Channel(const glm::vec3 &vScale, const glm::quat &qRotation, const glm::vec3 &vPosition, const float &fTime) :
    vScale(vScale),
    qRotation(qRotation),
    vPosition(vPosition),
    fTime(fTime)
{

}

/*
 *  Calculates the interpolation of the position
 *  at the time animation_time
 * */
glm::vec3 Channel::calcInterpolatedPosition(const Channel &channel1, const Channel &channel2, const unsigned int &uiAnimationDuration, const float &fFactor) const
{
    if(uiAnimationDuration == 1)
    {
        return channel1.vPosition;
    }

    const glm::vec3 vDelta = channel2.vPosition - channel1.vPosition;
    return channel1.vPosition + fFactor * vDelta;
}

glm::quat Channel::calcInterpolatedRotation(const Channel &channel1, const Channel &channel2, const unsigned int &uiAnimationDuration, const float &fFactor) const
{
    if(uiAnimationDuration == 1)
    {
        return channel1.qRotation;
    }

    return glm::normalize(glm::slerp(channel1.qRotation, channel2.qRotation, fFactor));
}

glm::vec3 Channel::calcInterpolatedScaling(const Channel &channel1, const Channel &channel2, const unsigned int &uiAnimationDuration, const float &fFactor) const
{
    if(uiAnimationDuration == 1)
    {
        return channel1.vScale;
    }

    const glm::vec3 vDelta = channel2.vScale - channel1.vScale;
    return channel1.vScale + fFactor * vDelta;
}


//  Animation

Animation::Animation(const unsigned int &uiDuration, const unsigned int &uiTicksPerSecond) :
    m_uiDuration(uiDuration),
    m_uiTicksPerSecond(uiTicksPerSecond)
{

}

Animation::~Animation()
{
    m_mapBoneChannels->clear();
}

void Animation::setChannel(const std::string &strBoneName, const unsigned int &uiTick, const Channel &channel)
{
    if(isBoneUsed(strBoneName) == true)
    {
        m_mapBoneChannels->at(strBoneName).setChannel(uiTick, channel);
    }
}
