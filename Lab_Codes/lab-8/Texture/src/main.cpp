#include "glad.h"
#include "glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath>
#include <iostream>
#include <algorithm>

const int WIDTH = 1000;
const int HEIGHT = 700;
const float PI = 3.14159265f;

unsigned int VAO, VBO, EBO, textureID, shaderProgram;

float yaw = 0.6f;
float pitch = 0.4f;
float cameraDistance = 5.0f;
float rotationSpeed = 0.7f;

bool dragging = false;
bool autoRotate = true;

double lastX = 0.0;
double lastY = 0.0;

struct Mat4 {
    float m[16];
};

Mat4 identity()
{
    Mat4 r = {};
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
    return r;
}

Mat4 multiply(Mat4 a, Mat4 b)
{
    Mat4 r = {};

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int k = 0; k < 4; k++) {
                r.m[col * 4 + row] +=
                    a.m[k * 4 + row] * b.m[col * 4 + k];
            }
        }
    }

    return r;
}

Mat4 perspective(float fov, float aspect,
                 float nearPlane, float farPlane)
{
    Mat4 r = {};
    float f = 1.0f / std::tan(fov / 2.0f);

    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (farPlane + nearPlane) /
              (nearPlane - farPlane);
    r.m[11] = -1.0f;
    r.m[14] = (2.0f * farPlane * nearPlane) /
              (nearPlane - farPlane);

    return r;
}

struct Vec3 {
    float x, y, z;
};

Vec3 normalize(Vec3 v)
{
    float len = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);

    if (len < 0.00001f)
        return {0, 0, 0};

    return {v.x/len, v.y/len, v.z/len};
}

Vec3 cross(Vec3 a, Vec3 b)
{
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

Vec3 subtract(Vec3 a, Vec3 b)
{
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}

float dot(Vec3 a, Vec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up)
{
    Vec3 f = normalize(subtract(target, eye));
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    Mat4 r = identity();

    r.m[0] = s.x;
    r.m[1] = u.x;
    r.m[2] = -f.x;

    r.m[4] = s.y;
    r.m[5] = u.y;
    r.m[6] = -f.y;

    r.m[8] = s.z;
    r.m[9] = u.z;
    r.m[10] = -f.z;

    r.m[12] = -dot(s, eye);
    r.m[13] = -dot(u, eye);
    r.m[14] = dot(f, eye);

    return r;
}

Mat4 rotationX(float a)
{
    Mat4 r = identity();

    r.m[5] = std::cos(a);
    r.m[6] = std::sin(a);
    r.m[9] = -std::sin(a);
    r.m[10] = std::cos(a);

    return r;
}

Mat4 rotationY(float a)
{
    Mat4 r = identity();

    r.m[0] = std::cos(a);
    r.m[2] = -std::sin(a);
    r.m[8] = std::sin(a);
    r.m[10] = std::cos(a);

    return r;
}

// ---------------- SHADERS ----------------

const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * worldPos;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 texColor = texture(ourTexture, TexCoord).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = 0.25 * texColor;
    vec3 diffuse = 0.85 * diff * texColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.25 * spec * vec3(1.0);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
)";

// ---------------- SHADER COMPILATION ----------------

unsigned int compileShader(GLenum type, const char* source)
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "Shader error:\n" << infoLog << '\n';
    }

    return shader;
}

unsigned int createShaderProgram()
{
    unsigned int vs =
        compileShader(GL_VERTEX_SHADER, vertexShaderSource);

    unsigned int fs =
        compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int program = glCreateProgram();

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    char infoLog[1024];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "Link error:\n" << infoLog << '\n';
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// ---------------- CUBE DATA ----------------
// Each vertex: position (3), normal (3), texture UV (2)

float vertices[] = {
    // Front face
    -1,-1, 1,  0,0,1,  0,0,
     1,-1, 1,  0,0,1,  1,0,
     1, 1, 1,  0,0,1,  1,1,
    -1, 1, 1,  0,0,1,  0,1,

    // Back face
     1,-1,-1,  0,0,-1, 0,0,
    -1,-1,-1,  0,0,-1, 1,0,
    -1, 1,-1,  0,0,-1, 1,1,
     1, 1,-1,  0,0,-1, 0,1,

    // Left face
    -1,-1,-1, -1,0,0,  0,0,
    -1,-1, 1, -1,0,0,  1,0,
    -1, 1, 1, -1,0,0,  1,1,
    -1, 1,-1, -1,0,0,  0,1,

    // Right face
     1,-1, 1,  1,0,0,  0,0,
     1,-1,-1,  1,0,0,  1,0,
     1, 1,-1,  1,0,0,  1,1,
     1, 1, 1,  1,0,0,  0,1,

    // Top face
    -1, 1, 1,  0,1,0,  0,0,
     1, 1, 1,  0,1,0,  1,0,
     1, 1,-1,  0,1,0,  1,1,
    -1, 1,-1,  0,1,0,  0,1,

    // Bottom face
    -1,-1,-1,  0,-1,0, 0,0,
     1,-1,-1,  0,-1,0, 1,0,
     1,-1, 1,  0,-1,0, 1,1,
    -1,-1, 1,  0,-1,0, 0,1
};

unsigned int indices[] = {
     0, 1, 2,  2, 3, 0,
     4, 5, 6,  6, 7, 4,
     8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20
};

// ---------------- TEXTURE LOADING ----------------

bool loadTexture(const char* filename)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(
        filename, &width, &height, &channels, 0);

    if (!data) {
        std::cout << "Cannot load texture: " << filename << '\n';
        std::cout << "Place texture.jpg in the project root folder.\n";
        return false;
    }

    GLenum format = GL_RGB;

    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return true;
}

// ---------------- INPUT ----------------

void mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            dragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            dragging = false;
        }
    }
}

void cursorPosition(GLFWwindow* window, double x, double y)
{
    if (!dragging) return;

    yaw += float(x - lastX) * 0.008f;
    pitch += float(y - lastY) * 0.008f;

    pitch = std::max(-1.5f, std::min(1.5f, pitch));

    lastX = x;
    lastY = y;
}

void scrollCallback(GLFWwindow* window, double x, double y)
{
    cameraDistance -= float(y) * 0.3f;
    cameraDistance = std::max(2.0f, std::min(12.0f, cameraDistance));
}

void keyCallback(GLFWwindow* window, int key, int scancode,
                 int action, int mods)
{
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_SPACE)
        autoRotate = !autoRotate;

    if (key == GLFW_KEY_R) {
        yaw = 0.6f;
        pitch = 0.4f;
        cameraDistance = 5.0f;
    }

    if (key == GLFW_KEY_UP)
        rotationSpeed += 0.2f;

    if (key == GLFW_KEY_DOWN)
        rotationSpeed -= 0.2f;
}

void framebufferSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ---------------- MAIN ----------------

int main()
{
    if (!glfwInit()) {
        std::cout << "GLFW initialization failed.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        WIDTH, HEIGHT, "Textured 3D Cube", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSize);
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, cursorPosition);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD initialization failed.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                 indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    if (!loadTexture("shuba-duck-know-your-meme.gif")) {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

    std::cout
        << "\nTEXTURED 3D CUBE\n"
        << "Left mouse drag: rotate camera\n"
        << "Mouse wheel: zoom\n"
        << "SPACE: toggle automatic rotation\n"
        << "UP/DOWN: change rotation speed\n"
        << "R: reset camera\n"
        << "ESC: exit\n";

    float angle = 0.0f;
    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - previousTime);
        previousTime = currentTime;

        if (autoRotate)
            angle += rotationSpeed * deltaTime;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if (width <= 0 || height <= 0) {
            glfwPollEvents();
            continue;
        }

        glViewport(0, 0, width, height);

        glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        Vec3 eye = {
            cameraDistance * std::cos(pitch) * std::sin(yaw),
            cameraDistance * std::sin(pitch),
            cameraDistance * std::cos(pitch) * std::cos(yaw)
        };

        Mat4 model = multiply(rotationY(angle), rotationX(angle * 0.7f));
        Mat4 view = lookAt(eye, {0,0,0}, {0,1,0});
        Mat4 projection = perspective(
            45.0f * PI / 180.0f,
            float(width) / float(height),
            0.1f, 100.0f);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, model.m);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, view.m);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "projection"),
            1, GL_FALSE, projection.m);

        glUniform3f(
            glGetUniformLocation(shaderProgram, "lightPos"),
            3.0f, 4.0f, 5.0f);

        glUniform3f(
            glGetUniformLocation(shaderProgram, "viewPos"),
            eye.x, eye.y, eye.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &textureID);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}