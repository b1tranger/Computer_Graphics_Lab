#ifndef GEODESIC_EDGES_H
#define GEODESIC_EDGES_H

#include "glad.h"
#include <glm/glm.hpp>
#include <vector>

class GeodesicEdges {
public:
    GeodesicEdges();
    ~GeodesicEdges();

    // Rebuild the 3 great-circle arcs between A, B, and C
    void build(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, int samplesPerArc = 120);

    // Render edge 0 (A->B, Red), edge 1 (B->C, Green), edge 2 (C->A, Gold)
    void render(unsigned int shaderProgram, int edgeIndex) const;

    glm::vec3 getEdgeColor(int edgeIndex) const {
        if (edgeIndex == 0) return glm::vec3(0.95f, 0.25f, 0.25f); // Red
        if (edgeIndex == 1) return glm::vec3(0.25f, 0.85f, 0.25f); // Green
        return glm::vec3(0.98f, 0.82f, 0.15f);                     // Gold/Yellow
    }

private:
    void setupArc(int index, const glm::vec3& start, const glm::vec3& end, int samples);
    void freeGLBuffers();

    unsigned int m_VAO[3] = {0, 0, 0};
    unsigned int m_VBO[3] = {0, 0, 0};
    int m_vertexCount[3] = {0, 0, 0};
};

#endif // GEODESIC_EDGES_H
