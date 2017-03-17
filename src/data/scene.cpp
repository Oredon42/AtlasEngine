#include "include/data/scene.h"
#include "include/data/model.h"
#include "include/data/lighting/skybox.h"

Scene::Scene(const std::string &path, GLfloat &render_time) :
    m_path(path),
    m_skybox(0),
    m_background_color(glm::vec3(0.f, 0.f, 0.f)),
    m_current_camera(0),
    m_render_time(render_time)
{

}

Scene::~Scene()
{
    for(size_t i = 0; i < m_roots.size(); ++i)
        delete m_roots[i];
    m_roots.clear();

    for(size_t i = 0; i < m_cameras.size(); ++i)
        delete m_cameras[i];
    m_cameras.clear();

    for(size_t i = 0; i < m_dirlights.size(); ++i)
        delete m_dirlights[i];
    m_dirlights.clear();

    for(size_t i = 0; i < m_pointlights.size(); ++i)
        delete m_pointlights[i];
    m_pointlights.clear();

    for(size_t i = 0; i < m_spotlights.size(); ++i)
        delete m_spotlights[i];
    m_spotlights.clear();

    if(m_skybox != 0)
        delete m_skybox;
}

/*
 * Draw the scene
 * with the shaders of the renderer
 * each Model will be drawn using
 * its Material shader
 * */
void Scene::draw(Shaders &shaders, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    /*Ray r = current_camera->getRay();
    Intersection i;
    Triangle t;
    r.intersectKdTree(m_kdtree, i, t);
    current_camera->setNear(i.t);*/

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(size_t i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            glUseProgram(shaders[i].getProgram());
            current_camera->sendDatas(shaders[i], window_width, window_height);

            for(size_t j = 0; j < m_dirlights.size(); ++j)
                m_dirlights[j]->sendDatas(shaders[i]);

            for(size_t j = 0; j < m_pointlights.size(); ++j)
                m_pointlights[j]->sendDatas(shaders[i]);

            for(size_t j = 0; j < m_spotlights.size(); ++j)
                m_spotlights[j]->sendDatas(shaders[i]);

            for(size_t j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->draw(shaders[i], render_time);
        }
    }
    /*if(m_skybox && m_skybox->isInitialised())
    {
        glDepthFunc(GL_LEQUAL);
        m_skybox->getShader().use();
        m_camera->sendDatas(m_skybox->getShader(), screen_width, screen_height);
        m_skybox->Draw(glm::mat4(glm::mat3(m_camera->getView())));
        glDepthFunc(GL_LESS);
    }*/

    /*m_kdshader.use();

    m_camera->sendDatas(m_kdshader, screen_width, screen_height);
    m_kdtree.Draw(m_kdshader);*/
}

/*
 * Draw the scene
 * with one shader
 * */
void Scene::draw(Shader &shader, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    /*Ray r = current_camera->getRay();
    Intersection i;
    Triangle t;
    r.intersectKdTree(m_kdtree, i, t);
    current_camera->setNear(i.t);*/

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(size_t i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            shader.use();
            current_camera->sendDatas(shader, window_width, window_height);

            for(size_t j = 0; j < m_dirlights.size(); ++j)
                m_dirlights[j]->sendDatas(shader);

            for(size_t j = 0; j < m_pointlights.size(); ++j)
                m_pointlights[j]->sendDatas(shader);

            for(size_t j = 0; j < m_spotlights.size(); ++j)
                m_spotlights[j]->sendDatas(shader);

            for(size_t j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->draw(shader, render_time);
        }
    }
}

/*
 * Draw the scene
 * with one shader
 * */
void Scene::drawNoLight(Shader &shader, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(size_t i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            shader.use();
            current_camera->sendDatas(shader, window_width, window_height);

            for(size_t j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->draw(shader, render_time);
        }
    }
}

void Scene::buildModelList()
{
    for(size_t i = 0; i < m_roots.size(); ++i)
        m_roots[i]->extractModels(m_models);
}

void Scene::buildKdTree()
{
    std::vector<Triangle *> T;

    for(size_t i = 0; i < NB_SHADER_TYPES; ++i)
    {
        for(size_t j = 0, m = 0; j < m_models[i].size(); ++j)
        {
            Model* actual_model = m_models[i][j];

            //  Pour tous les meshes du model
            for(size_t k = 0; k < actual_model->numberOfMeshes(); ++k)
            {
                Mesh* actual_mesh = actual_model->getMesh(k);
                size_t number_of_triangles = actual_mesh->numIndices();

                //  Pour tous les triangles du mesh
                for(size_t l = 0; l < number_of_triangles; l += 3, ++m)
                {
                    Triangle *t = actual_mesh->getTriangle(k, k+1, k+2);
                    t->mesh = j;
                    t->model = k;
                    t->side = l;

                    //  Ajout du triangle
                    m_kdtree.addTriangle(t);
                    T.push_back(t);

                    //  Recherche des extrémités de la bounding box de la scène
                    m_box.clipPoint(t->v1->Position);
                    m_box.clipPoint(t->v2->Position);
                    m_box.clipPoint(t->v3->Position);
                }
            }
        }
    }

    //  Création de la liste des évènements
    std::vector<Event> E;
    GLuint E_size;
    m_kdtree.createEventList(E_size, E);
    //  Construction de l'arbre
    m_kdtree.m_root = m_kdtree.RecBuild(T, m_box, E_size, E);
    //m_kdtree.m_root = m_kdtree.medbuild(T, B, 4);

    m_kdtree.setRootBox(m_box);

    //m_kdtree.setupMesh();   //  Affichage

    //m_kdtree_built = true;
}

void Scene::sendLightDatas(Shader &shader) const
{
    for(size_t i = 0; i < m_dirlights.size(); ++i)
        m_dirlights[i]->sendDatas(shader);

    for(size_t i = 0; i < m_pointlights.size(); ++i)
        m_pointlights[i]->sendDatas(shader);

    for(size_t i = 0; i < m_spotlights.size(); ++i)
        m_spotlights[i]->sendDatas(shader);
}

void Scene::sendViewSpaceLightDatas(const Shader &shader) const
{
    for(size_t i = 0; i < m_dirlights.size(); ++i)
        m_dirlights[i]->sendViewDatas(shader, m_cameras[m_current_camera]->getView());

    for(size_t i = 0; i < m_pointlights.size(); ++i)
        m_pointlights[i]->sendViewDatas(shader, m_cameras[m_current_camera]->getView());

    for(size_t i = 0; i < m_spotlights.size(); ++i)
        m_spotlights[i]->sendViewDatas(shader, m_cameras[m_current_camera]->getView());
}

SceneGraphNode *Scene::addSceneGraphNode(const std::string name, Model *model)
{
    SceneGraphNode *scene_graph_node = new SceneGraphNode(name);
    scene_graph_node->addModel(model);
    m_roots.back()->addChild(scene_graph_node);
    m_models[model->getMaterial()->getShaderTypeIndex()].push_back(model);

    return scene_graph_node;
}
