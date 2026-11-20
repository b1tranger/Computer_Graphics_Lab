#ifndef RAY_PICKER_H
#define RAY_PICKER_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

class RayPicker {
public:
    RayPicker();

    // Ray-sphere intersection from mouse screen coords
    bool pickPointOnSphere(double mouseX, double mouseY, int screenWidth, int screenHeight,
                           const glm::mat4& view, const glm::mat4& proj,
                           glm::vec3& outPoint);

    // Click handling for Question 2
    // Adds a point; when 3 valid points are collected, returns true and populates outA, outB, outC
    bool registerClick(const glm::vec3& pickedPoint, glm::vec3& outA, glm::vec3& outB, glm::vec3& outC,
                       std::string& statusMessage);

    void resetClicks();
    int getClickedCount() const { return static_cast<int>(m_clickedPoints.size()); }
    const std::vector<glm::vec3>& getClickedPoints() const { return m_clickedPoints; }

private:
    std::vector<glm::vec3> m_clickedPoints;
};

#endif // RAY_PICKER_H
