#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "glad.h"
#include <glm/glm.hpp>
#include <vector>

class SceneRenderer {
public:
    SceneRenderer();
    ~SceneRenderer();

    // Initialize coordinate axes, sphere grid rings, and marker cube buffers
    void init();

    // Render coordinate axes (X, Y, Z lines)
    void renderAxes(unsigned int lineShader) const;

    // Render sphere reference grid (great circles and latitude/longitude rings)
    void renderSphereGrid(unsigned int lineShader) const;

    // Render white cube markers at vertices A, B, and C
    void renderVertexMarkers(unsigned int surfaceShader, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
                             const glm::mat4& view, const glm::mat4& proj) const;

private:
    void setupAxes();
    void setupSphereGrid();
    void setupMarkerCube();
    void freeGLBuffers();

    // Axes
    unsigned int m_axesVAO = 0;
    unsigned int m_axesVBO = 0;

    // Sphere grid rings
    unsigned int m_gridVAO = 0;
    unsigned int m_gridVBO = 0;
    int m_gridVertexCount = 0;

    // Cube marker
    unsigned int m_cubeVAO = 0;
    unsigned int m_cubeVBO = 0;
    unsigned int m_cubeEBO = 0;
    int m_cubeIndexCount = 0;
};

#endif // SCENE_RENDERER_H
