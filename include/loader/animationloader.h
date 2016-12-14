#ifndef ANIMATIONLOADER_H
#define ANIMATIONLOADER_H

#include "include/openglincludes.h"
#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

class Scene;

class AnimatedModel;

class AnimationLoader
{
public:
    AnimationLoader();
    void load(const aiScene *ai_scene, const Scene *scene);
    void processAnimation(const aiAnimation *ai_animation, const Scene *scene, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLuint &time_in_ticks, const aiNode *ai_node);
    const aiNodeAnim *findNodeAnim(const aiAnimation *ai_animation, const std::string &node_name);
};

#endif // ANIMATIONLOADER_H
