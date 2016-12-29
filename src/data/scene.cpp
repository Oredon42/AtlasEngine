#include "include/data/scene.h"
#include "include/data/model.h"
#include "include/data/lighting/skybox.h"

Scene::Scene(const std::string &path, GLfloat &render_time) :
    m_path(path),
    m_nb_pointlights(0),
    m_nb_dirlights(0),
    m_nb_spotlights(0),
    m_skybox(0),
    m_background_color(glm::vec3(0.f, 0.f, 0.f)),
    m_current_camera(0),
    m_render_time(render_time)
{

}

Scene::~Scene()
{
    for(GLuint i = 0; i < (GLuint)(m_roots.size()); ++i)
        delete m_roots[i];
    m_roots.clear();

    for(GLuint i = 0; i < (GLuint)(m_cameras.size()); ++i)
        delete m_cameras[i];
    m_cameras.clear();

    for(GLuint i = 0; i < (GLuint)(m_lights.size()); ++i)
        delete m_lights[i];
    m_lights.clear();

    if(m_skybox != 0)
        delete m_skybox;
}

/*
 * Draw the scene using deferred rendering
 * with the shaders of the renderer
 * */
void Scene::draw(const Shaders &shaders, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            glUseProgram(shaders[i].getProgram());
            current_camera->sendDatas(shaders[i], window_width, window_height);

            for(GLuint j = 0; j < m_lights.size(); ++j)
                m_lights[j]->sendDatas(shaders[i]);

            for(GLuint j = 0; j < m_models[i].size(); ++j)
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
 * Draw the scene using deferred rendering
 * with one shader
 * */
void Scene::draw(const Shader &shader, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            shader.use();
            current_camera->sendDatas(shader, window_width, window_height);

            for(GLuint j = 0; j < m_lights.size(); ++j)
                m_lights[j]->sendDatas(shader);

            for(GLuint j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->draw(shader, render_time);
        }
    }
}

void Scene::buildKdTree()
{
    /*glm::vec3   v_min = m_models[0].m_m->m_vertices[0].Position,
                v_max = v_min;

    std::vector<Triangle *> T; //  vecteur de tous les triangles de la scène

    //  Ajout de tous les triangles de la scène dans la racine (m_root)
    //  Pour chaque model
    for(GLuint i = 0, l = 0; i < m_models.size(); ++i)
    {
        Model* actual_model = m_models[i];

        //  Pour tous les meshes du model
        for(GLuint j = 0; j < actual_model->numberOfMeshes(); ++j)
        {
            Mesh* actual_mesh = actual_model->getMesh(j);
            GLuint number_of_triangles = actual_mesh->m_indices.size();

            //  Pour tous les triangles du mesh
            for(GLuint k = 0; k < number_of_triangles; k += 3, ++l)
            {
                Triangle *t = new Triangle(&actual_mesh->m_vertices[actual_mesh->m_indices[k]], &actual_mesh->m_vertices[actual_mesh->m_indices[k + 1]], &actual_mesh->m_vertices[actual_mesh->m_indices[k + 2]], i, j, k);

                //  Ajout du triangle
                m_kdtree.m_T.push_back(t);

                //  Recherche des extrémités de la bounding box de la scène
                boxMinMax(v_min, v_max, t->v1->Position);
                boxMinMax(v_min, v_max, t->v2->Position);
                boxMinMax(v_min, v_max, t->v3->Position);
            }
        }
    }
    //  Strockage des adresses
    for(GLuint i = 0; i < m_kdtree.m_T.size(); ++i)
        T.push_back(m_kdtree.m_T[i]);

    //  Boîte englobante de la scène
    Box B;
    B.V[0] = v_min;
    B.V[1] = v_max;

    //  Création de la liste des évènements
    std::vector<Event> E;
    GLuint E_size;
    createEventList(E_size, E, m_kdtree.m_T);
    //  Construction de l'arbre
    m_kdtree.m_root = m_kdtree.RecBuild(T, B, E_size, E);
    //m_kdtree.m_root = m_kdtree.medbuild(T, B, 4);

    m_kdtree.setRootBox(B);

    m_kdtree.setupMesh();   //  Affichage

    m_kdtree_built = true;*/
}

void Scene::sendViewSpacePointLightDatas(const Shader &shader) const
{
    for(GLuint i = 0; i < m_lights.size(); ++i)
        m_lights[i]->sendViewDatas(shader, m_cameras[m_current_camera]->getView());
}

void Scene::addSceneGraphNode(const std::string name, Model *model)
{
    SceneGraphNode *scene_graph_node = new SceneGraphNode(name);
    scene_graph_node->insertModel(model);
    m_roots.back()->addChild(scene_graph_node);
    m_models[model->getMaterial()->getShaderTypeIndex()].push_back(model);
}
