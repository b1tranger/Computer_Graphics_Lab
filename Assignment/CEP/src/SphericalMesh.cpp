#include "SphericalMesh.h"
#include <algorithm>
#include <cstddef>

SphericalMesh::SphericalMesh() {
}

SphericalMesh::~SphericalMesh() {
    freeGLBuffers();
}

unsigned int SphericalMesh::getOrCreateMidpoint(unsigned int i1, unsigned int i2,
                                                std::unordered_map<uint64_t, unsigned int>& cache) {
    uint32_t minIdx = std::min(i1, i2);
    uint32_t maxIdx = std::max(i1, i2);
    uint64_t key = (static_cast<uint64_t>(minIdx) << 32) | static_cast<uint64_t>(maxIdx);

    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }

    // Midpoint projected onto unit sphere
    glm::vec3 p1 = m_vertices[i1].pos;
    glm::vec3 p2 = m_vertices[i2].pos;
    glm::vec3 midpoint = glm::normalize(p1 + p2);

    Vertex v;
    v.pos = midpoint;
    v.normal = midpoint; // For unit sphere centered at origin, normal = normalized position

    unsigned int newIndex = static_cast<unsigned int>(m_vertices.size());
    m_vertices.push_back(v);
    cache[key] = newIndex;

    return newIndex;
}

void SphericalMesh::build(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, int depth) {
    m_depth = std::max(0, std::min(7, depth));
    m_vertices.clear();
    m_indices.clear();

    // Initial 3 vertices
    glm::vec3 uA = glm::normalize(A);
    glm::vec3 uB = glm::normalize(B);
    glm::vec3 uC = glm::normalize(C);

    Vertex v0 = { uA, uA };
    Vertex v1 = { uB, uB };
    Vertex v2 = { uC, uC };

    m_vertices.push_back(v0);
    m_vertices.push_back(v1);
    m_vertices.push_back(v2);

    // Initial triangle indices
    std::vector<unsigned int> currentTriangles = { 0, 1, 2 };

    // Edge midpoint cache across all triangles to avoid duplicated vertices
    std::unordered_map<uint64_t, unsigned int> midpointCache;

    // Recursive subdivision
    for (int d = 0; d < m_depth; ++d) {
        std::vector<unsigned int> nextTriangles;
        nextTriangles.reserve(currentTriangles.size() * 4);

        for (size_t i = 0; i < currentTriangles.size(); i += 3) {
            unsigned int i0 = currentTriangles[i];
            unsigned int i1 = currentTriangles[i + 1];
            unsigned int i2 = currentTriangles[i + 2];

            unsigned int m01 = getOrCreateMidpoint(i0, i1, midpointCache);
            unsigned int m12 = getOrCreateMidpoint(i1, i2, midpointCache);
            unsigned int m20 = getOrCreateMidpoint(i2, i0, midpointCache);

            // 4 sub-triangles
            // 1) Corner 0
            nextTriangles.push_back(i0);
            nextTriangles.push_back(m01);
            nextTriangles.push_back(m20);

            // 2) Corner 1
            nextTriangles.push_back(i1);
            nextTriangles.push_back(m12);
            nextTriangles.push_back(m01);

            // 3) Corner 2
            nextTriangles.push_back(i2);
            nextTriangles.push_back(m20);
            nextTriangles.push_back(m12);

            // 4) Center
            nextTriangles.push_back(m01);
            nextTriangles.push_back(m12);
            nextTriangles.push_back(m20);
        }

        currentTriangles = std::move(nextTriangles);
    }

    m_indices = std::move(currentTriangles);
    setupGLBuffers();
}

void SphericalMesh::setupGLBuffers() {
    if (m_VAO == 0) {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
    }

    glBindVertexArray(m_VAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_DYNAMIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_DYNAMIC_DRAW);

    // Attribute 0: Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

    // Attribute 1: Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);
}

void SphericalMesh::freeGLBuffers() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        m_VAO = m_VBO = m_EBO = 0;
    }
}

void SphericalMesh::render() const {
    if (m_VAO != 0 && !m_indices.empty()) {
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
