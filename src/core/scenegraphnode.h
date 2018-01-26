#ifndef SCENEGRAPHNODE_H
#define SCENEGRAPHNODE_H

#include <string>
#include <vector>
#include <unordered_map>

#include <glew.h>
#include <glm/glm.hpp>

class Light;
class Model;
class Texture;
class Transform;
struct Bone;
struct VertexBoneData;

class SceneGraphNode
{
public:
    SceneGraphNode(const glm::mat4 &mGlobalInverseTransform = glm::mat4(1.f));
    virtual ~SceneGraphNode();

    void addChild(SceneGraphNode *pChild, const std::string &strKey);

    //  Getters
    inline SceneGraphNode *getParent() const{return m_pParent;}
    SceneGraphNode *getChild(const std::string &strKey);
    inline glm::mat4 getGlobalInverseTransform() const{return m_mGlobalInverseTransform;}

    inline Model *getModel(const unsigned int &i) const{return m_apModels[i];}
    inline Light *getLight(const unsigned int &i) const{return m_apLights[i];}

    inline size_t numberOfLights() const{return m_apLights.size();}
    inline size_t numberOfModels() const{return m_apModels.size();}

    //  Setters
    virtual void setTransform(const glm::mat4 &mTransform);
    virtual void translate(const glm::vec3 &vPosition);
    virtual void rotate(const glm::quat &qRotation);
    virtual void scale(const glm::vec3 &vScale);
    virtual void transform(const glm::mat4 &mTransform);

private:
    //  Tree relasionship
    SceneGraphNode *m_pParent;
    std::unordered_map<std::string, SceneGraphNode *> m_apChildren;

    //  Matrices
    Transform *m_pTransform;

    //  Data
    std::vector<Light *> m_apLights;
    std::vector<Model *> m_apModels;


    //  Private methods
    void spreadTransform();
};

#endif // SCENEGRAPHNODE_H
