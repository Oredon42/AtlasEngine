#include "include/data/geometry/proceduralmap.h"

#include <random>
#include <time.h>

ProceduralMap::ProceduralMap(const GLuint &N, const GLuint &map_side, const GLfloat &height_factor)
{
    //  Construction de la grille
    glm::vec2** noise = generatePerlinNoise(N);

    Vertex vertex;
    glm::vec3 vector;

    GLfloat factor      = (GLfloat)(map_side) / ((GLfloat)(N) - 2.f),
            factor_half = factor * 0.5f,
            factor_inv  = 1.f / factor;

    //  Calcul des indices & positions
    for(GLuint i = 0; i < map_side; ++i)
    {
        for(GLuint j = 0; j < map_side; ++j)
        {
            //  Calcul des positions
            vector.x = i;
            vector.z = j;

            //  Indices des 4 points dans la grille
            GLuint  x_noise_index       = i * factor_inv + 0.5f,
                    y_noise_index       = j * factor_inv + 0.5f,
                    x_noise_index_plus  = x_noise_index + 1.f,
                    y_noise_index_plus  = y_noise_index + 1.f;

            //  Vecteurs du point vers les points de la grille
            GLfloat dist_1_x = (x_noise_index       * factor - factor_half - j) * factor_inv,
                    dist_1_y = (y_noise_index       * factor - factor_half - i) * factor_inv,
                    dist_2_x = (x_noise_index       * factor - factor_half - j) * factor_inv,
                    dist_2_y = (y_noise_index_plus  * factor - factor_half - i) * factor_inv,
                    dist_3_x = (x_noise_index_plus  * factor - factor_half - j) * factor_inv,
                    dist_3_y = (y_noise_index       * factor - factor_half - i) * factor_inv,
                    dist_4_x = (x_noise_index_plus  * factor - factor_half - j) * factor_inv,
                    dist_4_y = (y_noise_index_plus  * factor - factor_half - i) * factor_inv;

            //  Vecteurs distance
            GLfloat scalar_1 = dist_1_y * noise[y_noise_index][x_noise_index].x             + dist_1_x * noise[y_noise_index][x_noise_index].y,
                    scalar_2 = dist_2_y * noise[y_noise_index_plus][x_noise_index].x        + dist_2_x * noise[y_noise_index_plus][x_noise_index].y,
                    scalar_3 = dist_3_y * noise[y_noise_index][x_noise_index_plus].x        + dist_3_x * noise[y_noise_index][x_noise_index_plus].y,
                    scalar_4 = dist_4_y * noise[y_noise_index_plus][x_noise_index_plus].x   + dist_4_x * noise[y_noise_index_plus][x_noise_index_plus].y;

            //  Interpolation bi-linéaire
            GLfloat t = (j - (x_noise_index * factor - factor_half)) * factor_inv,
                    u = (i - (y_noise_index * factor - factor_half)) * factor_inv;

            vector.y = factor_inv * ((1.f - t) * (1.f - u) * scalar_1 + (1.f - t) * u * scalar_2 + t * (1.f - u) * scalar_3 + t * u * scalar_4);
            vertex.Position = vector;
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);

            //  Ajout du vertex
            m_vertices.push_back(vertex);

            //  Calcul des indices
            if(i < map_side - 1 && j < map_side - 1)
            {
                m_indices.push_back(i * map_side + j);
                m_indices.push_back((i + 1) * map_side + j);
                m_indices.push_back(i * map_side + j + 1);
                m_indices.push_back(i * map_side + j + 1);
                m_indices.push_back((i + 1) * map_side + j);
                m_indices.push_back((i + 1) * map_side + j + 1);
            }
        }
    }


    //  Libération de la grille
    for(int i = 0; i < N; ++i)
        delete[] noise[i];
    delete[] noise;


    // Calcul des normales
    for(GLuint i = 0; i < map_side - 1; ++i)
    {
        for(GLuint j = 0; j < map_side - 1; ++j)
        {
            //  Calcul de la coordonnée verticale du vecteur
            GLfloat y1 = m_vertices[(i + 1) * map_side + j].Position.y - m_vertices[i * map_side + j].Position.y,
                    y2 = m_vertices[i * map_side + j + 1].Position.y - m_vertices[i * map_side + j].Position.y;

            glm::vec3 norm;

            //  Triangle 1
            norm.x = y1;
            norm.y = 1.f;
            norm.z = y2;

            norm = glm::normalize(norm);

            //  Ajout sur les vertices concernés
            m_vertices[i * map_side + j].Normal += norm;
            m_vertices[i * map_side + j + 1].Normal += norm;
            m_vertices[(i + 1) * map_side + j].Normal += norm;


            //  Triangle 2

            //  Calcul de la coordonnée verticale du vecteur
            y1 = m_vertices[i * map_side + j + 1].Position.y - m_vertices[(i + 1) * map_side + j + 1].Position.y;
            y2 = m_vertices[(i + 1) * map_side + j].Position.y - m_vertices[(i + 1) * map_side + j + 1].Position.y;

            norm.x = y2;
            norm.y = 1.f;
            norm.z = y1;

            norm = glm::normalize(norm);

            //  Ajout sur les vertices concernés
            m_vertices[(i + 1) * map_side + j + 1].Normal += norm;
            m_vertices[(i + 1) * map_side + j].Normal += norm;
            m_vertices[i * map_side + j + 1].Normal += norm;
        }
    }

    //  Normalisation
    for(int i = 0; i < map_side - 1; ++i)
    {
        for(int j = 0; j < map_side - 1; ++j)
        {
            glm::normalize(m_vertices[i * map_side + j].Normal);
            glm::normalize(m_vertices[i * map_side + j + 1].Normal);
            glm::normalize(m_vertices[(i + 1) * map_side + j].Normal);

            glm::normalize(m_vertices[(i + 1) * map_side + j + 1].Normal);
            glm::normalize(m_vertices[(i + 1) * map_side + j].Normal);
            glm::normalize(m_vertices[i * map_side + j + 1].Normal);
        }
    }
}

glm::vec2** ProceduralMap::generatePerlinNoise(const GLuint &N)
{
    std::srand(time(0));

    glm::vec2** noise = new glm::vec2*[N];

    for(GLuint i = 0; i < N; ++i)
        noise[i] = new glm::vec2[N];


    for(GLuint i = 0; i < N; ++i)
        for(GLuint j = 0; j < N; ++j)
            noise[i][j] = glm::normalize(glm::vec2(static_cast <GLfloat> (rand()) / ( static_cast <GLfloat> (RAND_MAX/(2))) - 1,
                                                    static_cast <GLfloat> (rand()) / ( static_cast <GLfloat> (RAND_MAX/(2))) - 1));

    return noise;
}
