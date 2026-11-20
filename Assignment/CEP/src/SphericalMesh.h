#ifndef SPHERICAL_MESH_H
#define SPHERICAL_MESH_H

#include "glad.h"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

class SphericalMesh {
public:
    SphericalMesh();
    ~SphericalMesh();

    // Rebuild mesh from 3 vertices on unit sphere with given subdivision depth
    void build(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, int depth);

    // Draw the surface using glDrawElements (indexed drawing)
    void render() const;

    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getTriangleCount() const { return m_indices.size() / 3; }
    int getSubdivisionDepth() const { return m_depth; }

private:
    unsigned int getOrCreateMidpoint(unsigned int i1, unsigned int i2,
                                     std::unordered_map<uint64_t, unsigned int>& cache);

    void setupGLBuffers();
    void freeGLBuffers();

    int m_depth = 5;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
};

#endif // SPHERICAL_MESH_H
