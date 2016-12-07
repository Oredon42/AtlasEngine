#ifndef ANIMATIONLOADER_H
#define ANIMATIONLOADER_H

#include "include/openglincludes.h"
#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

class AnimatedModel;

class AnimationLoader
{
public:
    AnimationLoader();
    void loadAnimations(const aiScene *ai_scene, const std::vector<AnimatedModel *> &animated_models);
    void processAnimation(const aiAnimation *ai_animation, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLuint &time_in_ticks, const aiNode *ai_node, const std::vector<AnimatedModel *> &animated_models);
    const aiNodeAnim *FindNodeAnim(const aiAnimation *ai_animation, const std::string &node_name);
};

#endif // ANIMATIONLOADER_H
