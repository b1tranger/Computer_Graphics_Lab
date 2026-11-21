#include "RayPicker.h"
#include "SphericalMath.h"
#include <cmath>
#include <sstream>

RayPicker::RayPicker() {
}

bool RayPicker::pickPointOnSphere(double mouseX, double mouseY, int screenWidth, int screenHeight,
                                 const glm::mat4& view, const glm::mat4& proj,
                                 glm::vec3& outPoint) {
    if (screenWidth <= 0 || screenHeight <= 0) return false;

    // Convert mouse to NDC [-1, 1]
    float x_ndc = (2.0f * static_cast<float>(mouseX)) / screenWidth - 1.0f;
    float y_ndc = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenHeight;

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::vec4 nearPoint = invVP * glm::vec4(x_ndc, y_ndc, -1.0f, 1.0f);
    glm::vec4 farPoint  = invVP * glm::vec4(x_ndc, y_ndc,  1.0f, 1.0f);

    if (std::abs(nearPoint.w) < 1e-6f || std::abs(farPoint.w) < 1e-6f) return false;

    nearPoint /= nearPoint.w;
    farPoint  /= farPoint.w;

    glm::vec3 rayOrigin = glm::vec3(nearPoint);
    glm::vec3 rayDir = glm::normalize(glm::vec3(farPoint - nearPoint));

    // Intersect with unit sphere: ||rayOrigin + t*rayDir||^2 = 1.0
    float a = glm::dot(rayDir, rayDir);
    float b = 2.0f * glm::dot(rayOrigin, rayDir);
    float c = glm::dot(rayOrigin, rayOrigin) - 1.0f;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return false; // Missed the sphere
    }

    float sqrtD = std::sqrt(discriminant);
    float t1 = (-b - sqrtD) / (2.0f * a);
    float t2 = (-b + sqrtD) / (2.0f * a);

    float t = -1.0f;
    if (t1 > 0.0f) t = t1;
    else if (t2 > 0.0f) t = t2;
    else return false;

    glm::vec3 hitPoint = rayOrigin + t * rayDir;
    outPoint = glm::normalize(hitPoint);
    return true;
}

bool RayPicker::registerClick(const glm::vec3& pickedPoint, glm::vec3& outA, glm::vec3& outB, glm::vec3& outC,
                              std::string& statusMessage) {
    m_clickedPoints.push_back(pickedPoint);

    if (m_clickedPoints.size() < 3) {
        std::ostringstream ss;
        ss << "Point " << m_clickedPoints.size() << " recorded: (" 
           << pickedPoint.x << ", " << pickedPoint.y << ", " << pickedPoint.z << "). Click next point.";
        statusMessage = ss.str();
        return false;
    }

    // We have 3 points: validate them
    glm::vec3 candA = m_clickedPoints[0];
    glm::vec3 candB = m_clickedPoints[1];
    glm::vec3 candC = m_clickedPoints[2];

    std::string err;
    if (!SphericalMath::validatePoints(candA, candB, candC, err)) {
        statusMessage = "REJECTED: " + err + " Resetting pick points. Please select 3 non-degenerate points.";
        m_clickedPoints.clear();
        return false;
    }

    outA = candA;
    outB = candB;
    outC = candC;
    m_clickedPoints.clear();
    statusMessage = "SUCCESS: Defined valid spherical triangle. Animating morph...";
    return true;
}

void RayPicker::resetClicks() {
    m_clickedPoints.clear();
}
