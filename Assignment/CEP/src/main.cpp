#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "SphericalMath.h"
#include "Camera.h"
#include "SphericalMesh.h"
#include "GeodesicEdges.h"
#include "SceneRenderer.h"
#include "RayPicker.h"

// Window dimensions
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camera
Camera camera(glm::vec3(0.0f), 2.8f, 45.0f, 30.0f);
double lastMouseX = SCR_WIDTH / 2.0;
double lastMouseY = SCR_HEIGHT / 2.0;
bool isDragging = false;
bool isPickingMode = false;

// Geometry state
glm::vec3 currentA(1.0f, 0.0f, 0.0f);
glm::vec3 currentB(0.0f, 1.0f, 0.0f);
glm::vec3 currentC(0.0f, 0.0f, 1.0f);

glm::vec3 targetA = currentA;
glm::vec3 targetB = currentB;
glm::vec3 targetC = currentC;

glm::vec3 startA = currentA;
glm::vec3 startB = currentB;
glm::vec3 startC = currentC;

bool isMorphing = false;
float morphProgress = 1.0f;
const float MORPH_DURATION = 1.0f; // 1 second smooth morph

int subdivisionDepth = 5;
bool wireframeMode = false;

// Components
SphericalMesh sphericalMesh;
GeodesicEdges geodesicEdges;
SceneRenderer sceneRenderer;
RayPicker rayPicker;

// Shader programs
unsigned int surfaceShaderProgram = 0;
unsigned int lineShaderProgram = 0;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

unsigned int createShaderProgram(const std::string& vertPath, const std::string& fragPath,
                                const std::string& fallbackVert, const std::string& fallbackFrag);
void updateGeometry(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, int depth);
void printTriangleReport(const SphericalMath::SphericalTriangleData& data);
void startMorphTo(const glm::vec3& newA, const glm::vec3& newB, const glm::vec3& newC);
bool saveScreenshotBMP(int width, int height, const std::string& filename);

// Default shader fallbacks if files cannot be read from current working directory
const std::string defaultSurfaceVert = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 FragPos;
out vec3 Normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const std::string defaultSurfaceFrag = R"(#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
void main() {
    float ambientStrength = 0.35;
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float backDiff = max(dot(-norm, lightDir), 0.0) * 0.35;
    diff = max(diff, backDiff);
    vec3 diffuse = diff * lightColor;
    float specularStrength = 0.65;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * lightColor;
    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
)";

const std::string defaultLineVert = R"(#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const std::string defaultLineFrag = R"(#version 330 core
out vec4 FragColor;
uniform vec3 lineColor;
void main() {
    FragColor = vec4(lineColor, 1.0);
}
)";

int main() {
    std::cout << "=================================================================\n";
    std::cout << "   COMPLEX ENGINEERING PROBLEM: SPHERICAL TRIANGLE VIEWER        \n";
    std::cout << "   Modern OpenGL (GLFW, GLAD, GLM) - Interactive 3D Application \n";
    std::cout << "=================================================================\n\n";

    // 1. Initial mathematical verification for the standard octant
    SphericalMath::SphericalTriangleData octantData = SphericalMath::computeTriangle(
        glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    std::cout << "[Step 1] Verifying standard right-angled octant:\n";
    printTriangleReport(octantData);

    // 2. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA for smooth antialiased great circle arcs

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
        "Spherical Triangle Viewer - Complex Engineering Problem", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // 3. Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);

    // 4. Compile shaders
    surfaceShaderProgram = createShaderProgram("shaders/surface.vert", "shaders/surface.frag",
                                              defaultSurfaceVert, defaultSurfaceFrag);
    lineShaderProgram = createShaderProgram("shaders/line.vert", "shaders/line.frag",
                                           defaultLineVert, defaultLineFrag);

    // 5. Initialize geometry and scene elements
    sceneRenderer.init();
    updateGeometry(currentA, currentB, currentC, subdivisionDepth);

    std::cout << "\n[Controls Guide]\n";
    std::cout << "  - Mouse Drag (Left Click) : Orbit / Arcball Rotate camera\n";
    std::cout << "  - Mouse Scroll            : Zoom camera in/out\n";
    std::cout << "  - W                       : Toggle Wireframe mode\n";
    std::cout << "  - '+' / '='               : Increase subdivision depth (up to 7)\n";
    std::cout << "  - '-'                     : Decrease subdivision depth (down to 0)\n";
    std::cout << "  - O                       : Reset to standard Octant\n";
    std::cout << "  - 1, 2, 3                 : Test general spherical triangles\n";
    std::cout << "  - P                       : Toggle Point-Picking mode (Question 2: click 3 points on sphere)\n";
    std::cout << "  - S                       : Save screenshot (screenshot.bmp)\n";
    std::cout << "  - R                       : Reset camera orientation\n";
    std::cout << "  - H                       : Print this controls help\n\n";

    // Timing
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    float fpsTimer = 0.0f;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // FPS and Title Update
        fpsTimer += deltaTime;
        frameCount++;
        if (fpsTimer >= 0.5f) {
            float currentFps = static_cast<float>(frameCount) / fpsTimer;
            SphericalMath::SphericalTriangleData curData = SphericalMath::computeTriangle(currentA, currentB, currentC);
            std::ostringstream titleStream;
            titleStream << "Spherical Triangle [Depth: " << subdivisionDepth 
                        << " | Verts: " << sphericalMesh.getVertexCount()
                        << " | Excess: " << std::fixed << std::setprecision(4) << curData.excess_rad
                        << " | Area: " << curData.area
                        << " | FPS: " << static_cast<int>(currentFps)
                        << (isPickingMode ? " | PICKING MODE" : "")
                        << "]";
            glfwSetWindowTitle(window, titleStream.str().c_str());
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        // Morphing animation (Question 2c)
        if (isMorphing) {
            morphProgress += deltaTime / MORPH_DURATION;
            if (morphProgress >= 1.0f) {
                morphProgress = 1.0f;
                isMorphing = false;
                currentA = targetA;
                currentB = targetB;
                currentC = targetC;
            } else {
                currentA = SphericalMath::slerp(startA, targetA, morphProgress);
                currentB = SphericalMath::slerp(startB, targetB, morphProgress);
                currentC = SphericalMath::slerp(startC, targetC, morphProgress);
            }
            updateGeometry(currentA, currentB, currentC, subdivisionDepth);
        }

        // Render pass
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspectRatio = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;

        // Dark background matching reference image
        glClearColor(0.12f, 0.12f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);

        // 1. Draw scene coordinate grid and axes
        glUseProgram(lineShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        sceneRenderer.renderSphereGrid(lineShaderProgram);
        sceneRenderer.renderAxes(lineShaderProgram);

        // 2. Draw spherical triangle surface with Blinn-Phong lighting
        glUseProgram(surfaceShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(surfaceShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(surfaceShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(surfaceShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Light setup (positioned in upper right front to create highlight seen in screenshot)
        glm::vec3 lightPos(2.5f, 2.5f, 3.0f);
        glUniform3fv(glGetUniformLocation(surfaceShaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glm::vec3 camPos = camera.getPosition();
        glUniform3fv(glGetUniformLocation(surfaceShaderProgram, "viewPos"), 1, glm::value_ptr(camPos));
        glUniform3f(glGetUniformLocation(surfaceShaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        // Vibrant sky blue matching reference screenshot
        glUniform3f(glGetUniformLocation(surfaceShaderProgram, "objectColor"), 0.16f, 0.52f, 0.95f);

        // Polygon mode (wireframe toggle)
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Zero Z-Fighting guarantee: push filled polygon depth slightly back
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);

        sphericalMesh.render();

        glDisable(GL_POLYGON_OFFSET_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // 3. Draw great-circle geodesic boundary edges (Red, Green, Gold)
        glUseProgram(lineShaderProgram);
        glLineWidth(3.0f);
        for (int e = 0; e < 3; ++e) {
            geodesicEdges.render(lineShaderProgram, e);
        }
        glLineWidth(1.0f);

        // 4. Draw white vertex markers at A, B, and C
        glUseProgram(surfaceShaderProgram);
        sceneRenderer.renderVertexMarkers(surfaceShaderProgram, currentA, currentB, currentC, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Auto-save screenshot for submission package if not exists
    saveScreenshotBMP(SCR_WIDTH, SCR_HEIGHT, "report/screenshot.bmp");

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void updateGeometry(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, int depth) {
    sphericalMesh.build(A, B, C, depth);
    geodesicEdges.build(A, B, C, 120);
}

void startMorphTo(const glm::vec3& newA, const glm::vec3& newB, const glm::vec3& newC) {
    startA = currentA;
    startB = currentB;
    startC = currentC;
    targetA = newA;
    targetB = newB;
    targetC = newC;
    morphProgress = 0.0f;
    isMorphing = true;

    SphericalMath::SphericalTriangleData data = SphericalMath::computeTriangle(newA, newB, newC);
    std::cout << "\n[Target Triangle Established]\n";
    printTriangleReport(data);
}

void printTriangleReport(const SphericalMath::SphericalTriangleData& data) {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  --------------------------------------------------------\n";
    std::cout << "  Vertex A : (" << data.A.x << ", " << data.A.y << ", " << data.A.z << ")\n";
    std::cout << "  Vertex B : (" << data.B.x << ", " << data.B.y << ", " << data.B.z << ")\n";
    std::cout << "  Vertex C : (" << data.C.x << ", " << data.C.y << ", " << data.C.z << ")\n";
    std::cout << "  Interior Angle A : " << std::setw(7) << data.angleA_deg << " deg (" << data.angleA_rad << " rad)\n";
    std::cout << "  Interior Angle B : " << std::setw(7) << data.angleB_deg << " deg (" << data.angleB_rad << " rad)\n";
    std::cout << "  Interior Angle C : " << std::setw(7) << data.angleC_deg << " deg (" << data.angleC_rad << " rad)\n";
    std::cout << "  Spherical Excess E = A + B + C - pi : " << data.excess_rad << " rad\n";
    std::cout << "  Spherical Triangle Area (E * R^2)  : " << data.area << "\n";
    std::cout << "  --------------------------------------------------------\n";
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

            // If in picking mode (Question 2), test ray-sphere intersection
            if (isPickingMode) {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                float aspect = (height > 0) ? (float)width / (float)height : 1.0f;
                glm::mat4 view = camera.getViewMatrix();
                glm::mat4 proj = camera.getProjectionMatrix(aspect);

                glm::vec3 pickedPoint;
                if (rayPicker.pickPointOnSphere(lastMouseX, lastMouseY, width, height, view, proj, pickedPoint)) {
                    glm::vec3 newA, newB, newC;
                    std::string status;
                    if (rayPicker.registerClick(pickedPoint, newA, newB, newC, status)) {
                        std::cout << "\n" << status << "\n";
                        startMorphTo(newA, newB, newC);
                    } else {
                        std::cout << status << "\n";
                    }
                } else {
                    std::cout << "Click missed unit sphere. Click directly on the sphere surface.\n";
                }
            }
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging && !isPickingMode) {
        float xoffset = static_cast<float>(xpos - lastMouseX);
        float yoffset = static_cast<float>(lastMouseY - ypos); // Invert y for standard trackball
        camera.processMouseMovement(xoffset, yoffset);
    }
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            wireframeMode = !wireframeMode;
            std::cout << "Wireframe mode: " << (wireframeMode ? "ENABLED" : "DISABLED") << "\n";
        } else if ((key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) && action == GLFW_PRESS) {
            if (subdivisionDepth < 7) {
                subdivisionDepth++;
                updateGeometry(currentA, currentB, currentC, subdivisionDepth);
                std::cout << "Subdivision depth increased to: " << subdivisionDepth 
                          << " (Triangles: " << sphericalMesh.getTriangleCount() 
                          << ", Vertices: " << sphericalMesh.getVertexCount() << ")\n";
            }
        } else if ((key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) && action == GLFW_PRESS) {
            if (subdivisionDepth > 0) {
                subdivisionDepth--;
                updateGeometry(currentA, currentB, currentC, subdivisionDepth);
                std::cout << "Subdivision depth decreased to: " << subdivisionDepth 
                          << " (Triangles: " << sphericalMesh.getTriangleCount() 
                          << ", Vertices: " << sphericalMesh.getVertexCount() << ")\n";
            }
        } else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
            std::cout << "Resetting to standard Right-Angled Octant...\n";
            startMorphTo(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        } else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            std::cout << "Setting Preset 1: Equilateral Spherical Triangle\n";
            startMorphTo(glm::normalize(glm::vec3(1.0f, 0.2f, 0.2f)),
                         glm::normalize(glm::vec3(0.2f, 1.0f, 0.2f)),
                         glm::normalize(glm::vec3(0.2f, 0.2f, 1.0f)));
        } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
            std::cout << "Setting Preset 2: Slender Acute Spherical Triangle\n";
            startMorphTo(glm::normalize(glm::vec3(0.0f, 1.0f, 0.2f)),
                         glm::normalize(glm::vec3(0.3f, 0.8f, 0.5f)),
                         glm::normalize(glm::vec3(-0.3f, 0.8f, 0.5f)));
        } else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
            std::cout << "Setting Preset 3: Wide Obtuse Spherical Triangle\n";
            startMorphTo(glm::normalize(glm::vec3(0.8f, 0.5f, 0.3f)),
                         glm::normalize(glm::vec3(-0.5f, 0.8f, 0.3f)),
                         glm::normalize(glm::vec3(0.0f, -0.6f, 0.8f)));
        } else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            isPickingMode = !isPickingMode;
            rayPicker.resetClicks();
            std::cout << "Point-Picking Mode (Q2): " 
                      << (isPickingMode ? "ACTIVE (Click 3 points on the sphere)" : "DISABLED (Camera rotation active)") << "\n";
        } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            saveScreenshotBMP(SCR_WIDTH, SCR_HEIGHT, "report/screenshot.bmp");
            std::cout << "Screenshot saved to report/screenshot.bmp\n";
        } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            camera.reset();
            std::cout << "Camera orientation reset.\n";
        } else if (key == GLFW_KEY_H && action == GLFW_PRESS) {
            std::cout << "\n[Controls Guide]\n"
                      << "  - Left Mouse Drag: Rotate Camera (when picking mode is off)\n"
                      << "  - Mouse Scroll: Zoom\n"
                      << "  - W: Toggle Wireframe\n"
                      << "  - +/-: Subdivision Depth\n"
                      << "  - O: Octant Preset\n"
                      << "  - 1, 2, 3: General Presets\n"
                      << "  - P: Toggle Q2 Point Picking\n"
                      << "  - S: Save Screenshot\n"
                      << "  - R: Reset Camera\n\n";
        }
    }
}

unsigned int createShaderProgram(const std::string& vertPath, const std::string& fragPath,
                                const std::string& fallbackVert, const std::string& fallbackFrag) {
    std::string vertCode = fallbackVert;
    std::string fragCode = fallbackFrag;

    std::ifstream vFile(vertPath);
    if (vFile.is_open()) {
        std::stringstream ss; ss << vFile.rdbuf(); vertCode = ss.str();
    }
    std::ifstream fFile(fragPath);
    if (fFile.is_open()) {
        std::stringstream ss; ss << fFile.rdbuf(); fragCode = ss.str();
    }

    const char* vShaderCode = vertCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compile error:\n" << infoLog << "\n";
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compile error:\n" << infoLog << "\n";
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader program linking error:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// 24-bit uncompressed BMP image writer
bool saveScreenshotBMP(int width, int height, const std::string& filename) {
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    // BMP header
    unsigned char fileHeader[14] = {
        'B', 'M',           // Signature
        0, 0, 0, 0,         // File size (will write below)
        0, 0, 0, 0,         // Reserved
        54, 0, 0, 0         // Offset to pixel data
    };

    unsigned char infoHeader[40] = {
        40, 0, 0, 0,        // Header size
        0, 0, 0, 0,         // Width
        0, 0, 0, 0,         // Height
        1, 0,               // Color planes
        24, 0,              // Bits per pixel
        0, 0, 0, 0,         // Compression (none)
        0, 0, 0, 0,         // Image size
        0, 0, 0, 0,         // Horizontal resolution
        0, 0, 0, 0,         // Vertical resolution
        0, 0, 0, 0,         // Colors in palette
        0, 0, 0, 0          // Important colors
    };

    int rowPadding = (4 - (width * 3) % 4) % 4;
    int dataSize = (width * 3 + rowPadding) * height;
    int fileSize = 54 + dataSize;

    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);

    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);

    infoHeader[20] = (unsigned char)(dataSize);
    infoHeader[21] = (unsigned char)(dataSize >> 8);
    infoHeader[22] = (unsigned char)(dataSize >> 16);
    infoHeader[23] = (unsigned char)(dataSize >> 24);

    file.write((const char*)fileHeader, 14);
    file.write((const char*)infoHeader, 40);

    unsigned char padding[3] = {0, 0, 0};
    for (int y = 0; y < height; ++y) {
        file.write((const char*)&pixels[y * width * 3], width * 3);
        if (rowPadding > 0) {
            file.write((const char*)padding, rowPadding);
        }
    }
    file.close();
    return true;
}
