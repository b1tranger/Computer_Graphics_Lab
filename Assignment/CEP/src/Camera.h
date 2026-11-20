#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

class Camera {
public:
    Camera(glm::vec3 target = glm::vec3(0.0f), float distance = 2.8f, float yaw = 45.0f, float pitch = 30.0f)
        : m_target(target), m_distance(distance), m_yaw(yaw), m_pitch(pitch),
          m_defaultDistance(distance), m_defaultYaw(yaw), m_defaultPitch(pitch) {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(m_position, m_target, m_up);
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
    }

    glm::vec3 getPosition() const {
        return m_position;
    }

    float getFOV() const {
        return m_fov;
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        float sensitivity = 0.25f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (constrainPitch) {
            m_pitch = std::max(-89.0f, std::min(89.0f, m_pitch));
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yoffset) {
        m_distance -= yoffset * 0.2f;
        m_distance = std::max(1.2f, std::min(10.0f, m_distance));
        updateCameraVectors();
    }

    void reset() {
        m_distance = m_defaultDistance;
        m_yaw = m_defaultYaw;
        m_pitch = m_defaultPitch;
        m_fov = 45.0f;
        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        float radYaw = glm::radians(m_yaw);
        float radPitch = glm::radians(m_pitch);

        glm::vec3 offset;
        offset.x = m_distance * std::cos(radPitch) * std::sin(radYaw);
        offset.y = m_distance * std::sin(radPitch);
        offset.z = m_distance * std::cos(radPitch) * std::cos(radYaw);

        m_position = m_target + offset;
        m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_distance;
    float m_yaw;
    float m_pitch;
    float m_fov = 45.0f;

    float m_defaultDistance;
    float m_defaultYaw;
    float m_defaultPitch;
};

#endif // CAMERA_H
