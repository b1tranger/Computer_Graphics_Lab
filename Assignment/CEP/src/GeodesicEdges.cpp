#include "GeodesicEdges.h"
#include "SphericalMath.h"

GeodesicEdges::GeodesicEdges() {
}

GeodesicEdges::~GeodesicEdges() {
    freeGLBuffers();
}

void GeodesicEdges::freeGLBuffers() {
    for (int i = 0; i < 3; ++i) {
        if (m_VAO[i] != 0) {
            glDeleteVertexArrays(1, &m_VAO[i]);
            glDeleteBuffers(1, &m_VBO[i]);
            m_VAO[i] = m_VBO[i] = 0;
            m_vertexCount[i] = 0;
        }
    }
}

void GeodesicEdges::setupArc(int index, const glm::vec3& start, const glm::vec3& end, int samples) {
    if (m_VAO[index] == 0) {
        glGenVertexArrays(1, &m_VAO[index]);
        glGenBuffers(1, &m_VBO[index]);
    }

    std::vector<glm::vec3> points;
    points.reserve(samples + 1);

    glm::vec3 uStart = glm::normalize(start);
    glm::vec3 uEnd = glm::normalize(end);

    for (int i = 0; i <= samples; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(samples);
        glm::vec3 p = SphericalMath::slerp(uStart, uEnd, t);
        // Slight radial offset (1.002) to guarantee clean visibility over surface without z-fighting
        points.push_back(p * 1.002f);
    }

    m_vertexCount[index] = static_cast<int>(points.size());

    glBindVertexArray(m_VAO[index]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[index]);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void GeodesicEdges::build(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, int samplesPerArc) {
    setupArc(0, A, B, samplesPerArc); // Edge AB
    setupArc(1, B, C, samplesPerArc); // Edge BC
    setupArc(2, C, A, samplesPerArc); // Edge CA
}

void GeodesicEdges::render(unsigned int shaderProgram, int edgeIndex) const {
    if (edgeIndex < 0 || edgeIndex > 2 || m_VAO[edgeIndex] == 0) return;

    GLint colorLoc = glGetUniformLocation(shaderProgram, "lineColor");
    glm::vec3 col = getEdgeColor(edgeIndex);
    glUniform3f(colorLoc, col.r, col.g, col.b);

    glBindVertexArray(m_VAO[edgeIndex]);
    glDrawArrays(GL_LINE_STRIP, 0, m_vertexCount[edgeIndex]);
    glBindVertexArray(0);
}
