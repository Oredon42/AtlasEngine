#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>

#include <unordered_map>
#include <glm/glm.hpp>

/*
 * A channel describes the transform
 * of an animated bone at a tick
 * */
struct Channel
{
    glm::vec3 vScale;
    glm::quat qRotation;
    glm::vec3 vPosition;
    float fTime;

    Channel();
    Channel(const glm::vec3 &vScale, const glm::quat &qRotation, const glm::vec3 &vPosition, const float &fTime);

    static glm::vec3 calcInterpolatedPosition(const Channel &channel1, const Channel &channel2, const unsigned int &uiAnimationDuration, const float &fFactor);
    static glm::quat calcInterpolatedRotation(const Channel &channel1, const Channel &channel2, const unsigned int &uiAnimationDuration, const float &fFactor);
    static glm::vec3 calcInterpolatedScaling(const Channel &channel1, const Channel &channel2, const unsigned int &uiAnimationDuration, const float &fFactor);
};

/*
 * An animation stores an Armature's Bones transfom
 * */
class Animation
{
    /*
     * Contains an array of Channels
     * for a single bone
     * */
    struct BoneChannels
    {
        Channel *aBoneChannels;

        BoneChannels(const unsigned int &uiDuration) {aBoneChannels = new Channel[uiDuration];}
        ~BoneChannels() {delete[] aBoneChannels;}

        inline void setChannel(const unsigned int &uiTick, const Channel &channel){aBoneChannels[uiTick] = channel;}
        inline Channel getChannel(const unsigned int &uiTick) const{return aBoneChannels[uiTick];}
    };

public:
    Animation(const unsigned int &uiDuration, const unsigned int &uiTicksPerSecond);
    ~Animation();

    //  Getters
    inline Channel getChannel(const std::string &strBoneName, const unsigned int &uiTick) const{return m_mapBoneChannels->at(strBoneName).getChannel(uiTick);}
    inline unsigned int getTicksPerSecond() const{return m_uiTicksPerSecond;}
    inline unsigned int getDuration() const{return m_uiDuration;}
    inline bool isBoneUsed(const std::string &strBoneName) const{return m_mapBoneChannels->find(strBoneName) != m_mapBoneChannels->end();}

    //  Setters
    inline void setDuration(const unsigned int &uiDuration) {m_uiDuration = uiDuration;}
    inline void setTicksPerSecond(const unsigned int &uiTicksPerSecond) {m_uiTicksPerSecond = uiTicksPerSecond;}
    void setChannel(const std::string &strBoneName, const unsigned int &uiTick, const Channel &channel);

private:
    unsigned int m_uiDuration;
    unsigned int m_uiTicksPerSecond;

    std::unordered_map<std::string, BoneChannels> *m_mapBoneChannels;
};

#endif // ANIMATION_H
