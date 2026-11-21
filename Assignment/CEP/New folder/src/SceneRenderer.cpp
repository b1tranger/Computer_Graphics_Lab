#include "SceneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

SceneRenderer::SceneRenderer() {
}

SceneRenderer::~SceneRenderer() {
    freeGLBuffers();
}

void SceneRenderer::freeGLBuffers() {
    if (m_axesVAO) { glDeleteVertexArrays(1, &m_axesVAO); glDeleteBuffers(1, &m_axesVBO); m_axesVAO = m_axesVBO = 0; }
    if (m_gridVAO) { glDeleteVertexArrays(1, &m_gridVAO); glDeleteBuffers(1, &m_gridVBO); m_gridVAO = m_gridVBO = 0; }
    if (m_cubeVAO) { glDeleteVertexArrays(1, &m_cubeVAO); glDeleteBuffers(1, &m_cubeVBO); glDeleteBuffers(1, &m_cubeEBO); m_cubeVAO = m_cubeVBO = m_cubeEBO = 0; }
}

void SceneRenderer::init() {
    setupAxes();
    setupSphereGrid();
    setupMarkerCube();
}

void SceneRenderer::setupAxes() {
    // 3 lines extending along positive and negative axes (or origin to 1.6)
    float len = 1.6f;
    std::vector<glm::vec3> axesLines = {
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(len, 0.0f, 0.0f), // X
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, len, 0.0f), // Y
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, len)  // Z
    };

    glGenVertexArrays(1, &m_axesVAO);
    glGenBuffers(1, &m_axesVBO);

    glBindVertexArray(m_axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axesLines.size() * sizeof(glm::vec3), axesLines.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBindVertexArray(0);
}

void SceneRenderer::setupSphereGrid() {
    // Generate great-circle rings and latitude/longitude rings
    std::vector<glm::vec3> gridLines;
    const int segments = 72;
    const float twoPi = glm::two_pi<float>();

    auto addCircle = [&](const glm::vec3& center, float radius, const glm::vec3& normal) {
        glm::vec3 u = (std::abs(normal.z) < 0.999f) ? glm::normalize(glm::cross(normal, glm::vec3(0, 0, 1)))
                                                     : glm::normalize(glm::cross(normal, glm::vec3(1, 0, 0)));
        glm::vec3 v = glm::cross(normal, u);

        glm::vec3 prev = center + radius * (std::cos(0.0f) * u + std::sin(0.0f) * v);
        for (int i = 1; i <= segments; ++i) {
            float angle = (static_cast<float>(i) / segments) * twoPi;
            glm::vec3 curr = center + radius * (std::cos(angle) * u + std::sin(angle) * v);
            gridLines.push_back(prev);
            gridLines.push_back(curr);
            prev = curr;
        }
    };

    // 1) Major great circles (XY, YZ, ZX planes)
    addCircle(glm::vec3(0.0f), 1.0f, glm::vec3(0, 0, 1)); // Equator / XY
    addCircle(glm::vec3(0.0f), 1.0f, glm::vec3(1, 0, 0)); // Meridian / YZ
    addCircle(glm::vec3(0.0f), 1.0f, glm::vec3(0, 1, 0)); // Meridian / ZX

    // 2) Diagonal great circles (for full sphere structure)
    addCircle(glm::vec3(0.0f), 1.0f, glm::normalize(glm::vec3(1, 1, 0)));
    addCircle(glm::vec3(0.0f), 1.0f, glm::normalize(glm::vec3(1, -1, 0)));
    addCircle(glm::vec3(0.0f), 1.0f, glm::normalize(glm::vec3(0, 1, 1)));
    addCircle(glm::vec3(0.0f), 1.0f, glm::normalize(glm::vec3(0, 1, -1)));

    // 3) Latitude rings
    std::vector<float> lats = { -60.0f, -30.0f, 30.0f, 60.0f };
    for (float latDeg : lats) {
        float phi = glm::radians(latDeg);
        float r = std::cos(phi);
        float y = std::sin(phi);
        addCircle(glm::vec3(0.0f, y, 0.0f), r, glm::vec3(0, 1, 0));
    }

    m_gridVertexCount = static_cast<int>(gridLines.size());

    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers(1, &m_gridVBO);

    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridLines.size() * sizeof(glm::vec3), gridLines.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBindVertexArray(0);
}

void SceneRenderer::setupMarkerCube() {
    // Small unit cube centered at origin
    float s = 0.022f; // half size
    struct CubeVertex { glm::vec3 pos; glm::vec3 normal; };
    std::vector<CubeVertex> vertices = {
        // Front
        { {-s, -s,  s}, {0, 0, 1} }, { { s, -s,  s}, {0, 0, 1} }, { { s,  s,  s}, {0, 0, 1} }, { {-s,  s,  s}, {0, 0, 1} },
        // Back
        { { s, -s, -s}, {0, 0, -1} }, { {-s, -s, -s}, {0, 0, -1} }, { {-s,  s, -s}, {0, 0, -1} }, { { s,  s, -s}, {0, 0, -1} },
        // Left
        { {-s, -s, -s}, {-1, 0, 0} }, { {-s, -s,  s}, {-1, 0, 0} }, { {-s,  s,  s}, {-1, 0, 0} }, { {-s,  s, -s}, {-1, 0, 0} },
        // Right
        { { s, -s,  s}, {1, 0, 0} }, { { s, -s, -s}, {1, 0, 0} }, { { s,  s, -s}, {1, 0, 0} }, { { s,  s,  s}, {1, 0, 0} },
        // Top
        { {-s,  s,  s}, {0, 1, 0} }, { { s,  s,  s}, {0, 1, 0} }, { { s,  s, -s}, {0, 1, 0} }, { {-s,  s, -s}, {0, 1, 0} },
        // Bottom
        { {-s, -s, -s}, {0, -1, 0} }, { { s, -s, -s}, {0, -1, 0} }, { { s, -s,  s}, {0, -1, 0} }, { {-s, -s,  s}, {0, -1, 0} }
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  0, 2, 3,       // Front
        4, 5, 6,  4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,     // Left
        12, 13, 14, 12, 14, 15,  // Right
        16, 17, 18, 16, 18, 19,  // Top
        20, 21, 22, 20, 22, 23   // Bottom
    };
    m_cubeIndexCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    glGenBuffers(1, &m_cubeEBO);

    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CubeVertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, normal));

    glBindVertexArray(0);
}

void SceneRenderer::renderAxes(unsigned int lineShader) const {
    GLint colorLoc = glGetUniformLocation(lineShader, "lineColor");
    // Light gray / off-white axes lines
    glUniform3f(colorLoc, 0.75f, 0.75f, 0.78f);

    glBindVertexArray(m_axesVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
}

void SceneRenderer::renderSphereGrid(unsigned int lineShader) const {
    GLint colorLoc = glGetUniformLocation(lineShader, "lineColor");
    // Faint subtle gray grid rings
    glUniform3f(colorLoc, 0.32f, 0.34f, 0.38f);

    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, m_gridVertexCount);
    glBindVertexArray(0);
}

void SceneRenderer::renderVertexMarkers(unsigned int surfaceShader, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
                                       const glm::mat4& view, const glm::mat4& proj) const {
    GLint modelLoc = glGetUniformLocation(surfaceShader, "model");
    GLint baseColorLoc = glGetUniformLocation(surfaceShader, "objectColor");

    // White markers
    glUniform3f(baseColorLoc, 0.95f, 0.95f, 0.95f);

    glm::vec3 pts[3] = { A, B, C };
    glBindVertexArray(m_cubeVAO);

    for (int i = 0; i < 3; ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pts[i]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, m_cubeIndexCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}
