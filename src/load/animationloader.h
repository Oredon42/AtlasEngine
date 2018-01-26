#ifndef ANIMATIONLOADER_H
#define ANIMATIONLOADER_H

#include <string>

class Database;
class Animation;
class AnimatedModel;
struct aiScene;
struct aiAnimation;
struct aiNodeAnim;
struct aiNode;

/*
 * This class will load Animations
 * from an aiScene
 * and store them in Database
 * */
class AnimationLoader
{
public:
    AnimationLoader();

    void load(Database *pDatabase, const aiScene *pAiScene);

private:
    void processBonesChannels(Animation *pAnimation, const aiNode *pBone, const aiAnimation *pAiAnimation, const unsigned int &uiCurrentTick);
    const aiNodeAnim *findNodeAnim(const aiAnimation *pAiAnimation, const std::string &strNodeName);
};

#endif // ANIMATIONLOADER_H
