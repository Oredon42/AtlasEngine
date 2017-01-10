#ifndef PROCEDURALMAP_H
#define PROCEDURALMAP_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "mesh.h"

class ProceduralMap : public Mesh
{
public:
    ProceduralMap(const GLuint &N, const GLuint &map_side, const GLfloat &height_factor);

private:
    glm::vec2 **generatePerlinNoise(const GLuint &N);

    void setup();
};

#endif // PROCEDURALMAP_H
