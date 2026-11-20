#ifndef SPHERICAL_MATH_H
#define SPHERICAL_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>

namespace SphericalMath {

// Clamp helper
inline float clampf(float val, float minVal, float maxVal) {
    return std::max(minVal, std::min(maxVal, val));
}

// Great-circle distance (angle in radians between two unit vectors)
inline float greatCircleDistance(const glm::vec3& u, const glm::vec3& v) {
    float dot = clampf(glm::dot(u, v), -1.0f, 1.0f);
    return std::acos(dot);
}

// Spherical Linear Interpolation (SLERP) between two unit vectors
inline glm::vec3 slerp(const glm::vec3& a, const glm::vec3& b, float t) {
    float dot = clampf(glm::dot(a, b), -1.0f, 1.0f);
    float theta = std::acos(dot);
    if (std::abs(theta) < 1e-5f) {
        return glm::normalize(glm::mix(a, b, t));
    }
    float sinTheta = std::sin(theta);
    float w1 = std::sin((1.0f - t) * theta) / sinTheta;
    float w2 = std::sin(t * theta) / sinTheta;
    return glm::normalize(w1 * a + w2 * b);
}

// Compute interior angle at vertex A between geodesic arcs AB and AC
// Uses tangent vectors on sphere surface at A
inline float computeInteriorAngle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    // Tangent along arc AB at A: component of B orthogonal to A
    glm::vec3 tAB = b - glm::dot(a, b) * a;
    // Tangent along arc AC at A: component of C orthogonal to A
    glm::vec3 tAC = c - glm::dot(a, c) * a;
    
    float lenAB = glm::length(tAB);
    float lenAC = glm::length(tAC);
    if (lenAB < 1e-6f || lenAC < 1e-6f) {
        return 0.0f;
    }
    tAB /= lenAB;
    tAC /= lenAC;
    
    float dot = clampf(glm::dot(tAB, tAC), -1.0f, 1.0f);
    return std::acos(dot);
}

// Structure holding spherical triangle geometric measurements
struct SphericalTriangleData {
    glm::vec3 A, B, C;
    float angleA_rad, angleB_rad, angleC_rad;
    float angleA_deg, angleB_deg, angleC_deg;
    float excess_rad;       // Spherical excess E = A + B + C - PI
    float area;             // Area on unit sphere = E * R^2 = E
    float arcAB_rad, arcBC_rad, arcCA_rad; // Side lengths
};

// Compute all spherical measurements for vertices A, B, C on unit sphere
inline SphericalTriangleData computeTriangle(glm::vec3 A, glm::vec3 B, glm::vec3 C) {
    A = glm::normalize(A);
    B = glm::normalize(B);
    C = glm::normalize(C);

    SphericalTriangleData data;
    data.A = A;
    data.B = B;
    data.C = C;

    data.angleA_rad = computeInteriorAngle(A, B, C);
    data.angleB_rad = computeInteriorAngle(B, C, A);
    data.angleC_rad = computeInteriorAngle(C, A, B);

    const float rad2deg = 180.0f / glm::pi<float>();
    data.angleA_deg = data.angleA_rad * rad2deg;
    data.angleB_deg = data.angleB_rad * rad2deg;
    data.angleC_deg = data.angleC_rad * rad2deg;

    data.excess_rad = data.angleA_rad + data.angleB_rad + data.angleC_rad - glm::pi<float>();
    data.area = std::max(0.0f, data.excess_rad);

    data.arcAB_rad = greatCircleDistance(A, B);
    data.arcBC_rad = greatCircleDistance(B, C);
    data.arcCA_rad = greatCircleDistance(C, A);

    return data;
}

// Input validation: check for collinearity, antipodal points, or coincident points
inline bool validatePoints(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, std::string& errorMsg) {
    // Check coincident
    if (glm::distance(A, B) < 1e-3f || glm::distance(B, C) < 1e-3f || glm::distance(C, A) < 1e-3f) {
        errorMsg = "Degenerate Input: Two or more selected points are identical/coincident.";
        return false;
    }
    // Check antipodal points
    if (glm::dot(A, B) < -0.999f || glm::dot(B, C) < -0.999f || glm::dot(C, A) < -0.999f) {
        errorMsg = "Degenerate Input: Points contain antipodal pairs (great circle is ill-defined).";
        return false;
    }
    // Check collinearity (coplanar with center of sphere)
    glm::vec3 normal = glm::cross(B - A, C - A);
    float scalarTripleProduct = std::abs(glm::dot(A, glm::cross(B, C)));
    if (scalarTripleProduct < 1e-4f || glm::length(normal) < 1e-4f) {
        errorMsg = "Degenerate Input: Selected points are collinear / lie on the same great circle.";
        return false;
    }
    return true;
}

} // namespace SphericalMath

#endif // SPHERICAL_MATH_H
