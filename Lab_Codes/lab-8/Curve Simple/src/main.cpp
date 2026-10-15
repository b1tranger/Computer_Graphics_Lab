#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <vector>
#include <cmath>

// A 2D point in OpenGL normalized device coordinates
struct Point
{
    float x, y;
};

std::vector<Point> points;
std::vector<Point> curve;

bool curveReady = false;

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core

uniform vec3 uColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(uColor, 1.0);
}
)";

unsigned int shaderProgram;
unsigned int VAO, VBO;
int colorLocation;

// Convert mouse position to OpenGL coordinates
void mouse_button_callback(
    GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT &&
        action == GLFW_PRESS)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        if (width == 0 || height == 0)
            return;

        Point p;

        p.x = 2.0f * (float)mouseX / width - 1.0f;
        p.y = 1.0f - 2.0f * (float)mouseY / height;

        points.push_back(p);

        // New point means the curve needs to be generated again
        curveReady = false;
        curve.clear();

        std::cout << "Point " << points.size()
                  << ": (" << p.x << ", " << p.y << ")\n";
    }
}

// Catmull-Rom spline interpolation
Point catmullRom(
    Point p0, Point p1, Point p2, Point p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    Point result;

    result.x = 0.5f * (
        2.0f * p1.x +
        (-p0.x + p2.x) * t +
        (2.0f*p0.x - 5.0f*p1.x
         + 4.0f*p2.x - p3.x) * t2 +
        (-p0.x + 3.0f*p1.x
         - 3.0f*p2.x + p3.x) * t3
    );

    result.y = 0.5f * (
        2.0f * p1.y +
        (-p0.y + p2.y) * t +
        (2.0f*p0.y - 5.0f*p1.y
         + 4.0f*p2.y - p3.y) * t2 +
        (-p0.y + 3.0f*p1.y
         - 3.0f*p2.y + p3.y) * t3
    );

    return result;
}

// Generate a smooth curve through all control points
void generateCurve()
{
    curve.clear();

    if (points.size() < 2)
    {
        std::cout << "Click at least 2 points first.\n";
        return;
    }

    const int samplesPerSegment = 40;

    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        Point p0 = (i == 0)
            ? points[i] : points[i - 1];

        Point p1 = points[i];

        Point p2 = points[i + 1];

        Point p3 = (i + 2 < points.size())
            ? points[i + 2] : points[i + 1];

        for (int j = 0; j < samplesPerSegment; ++j)
        {
            float t = (float)j / samplesPerSegment;

            curve.push_back(
                catmullRom(p0, p1, p2, p3, t)
            );
        }
    }

    // Include the last control point
    curve.push_back(points.back());

    curveReady = true;

    std::cout << "Curve generated using "
              << points.size() << " points.\n";
}

// Keyboard controls
void key_callback(
    GLFWwindow* window, int key,
    int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_ENTER ||
        key == GLFW_KEY_KP_ENTER)
    {
        generateCurve();
    }

    if (key == GLFW_KEY_C)
    {
        points.clear();
        curve.clear();
        curveReady = false;

        std::cout << "All points cleared.\n";
    }
}

// Compile a shader
unsigned int compileShader(
    GLenum type, const char* source)
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(
            shader, 512, nullptr, infoLog);

        std::cout << "Shader compilation error:\n"
                  << infoLog << '\n';
    }

    return shader;
}

// Create the shader program
unsigned int createShaderProgram()
{
    unsigned int vertexShader =
        compileShader(GL_VERTEX_SHADER,
                      vertexShaderSource);

    unsigned int fragmentShader =
        compileShader(GL_FRAGMENT_SHADER,
                      fragmentShaderSource);

    unsigned int program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(
            program, 512, nullptr, infoLog);

        std::cout << "Shader linking error:\n"
                  << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Upload points and draw them
void drawPoints(
    const std::vector<Point>& data,
    GLenum mode, float r, float g, float b,
    float pointSize = 1.0f)
{
    if (data.empty())
        return;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        data.size() * sizeof(Point),
        data.data(),
        GL_DYNAMIC_DRAW
    );

    glUniform3f(colorLocation, r, g, b);
    glPointSize(pointSize);

    glDrawArrays(
        mode, 0, static_cast<GLsizei>(data.size())
    );
}

void framebuffer_size_callback(
    GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        900, 700, "Interactive Curve Drawing",
        nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(
        window, framebuffer_size_callback);

    glfwSetMouseButtonCallback(
        window, mouse_button_callback);

    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 900, 700);

    shaderProgram = createShaderProgram();

    colorLocation = glGetUniformLocation(
        shaderProgram, "uColor");

    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        sizeof(Point), (void*)0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::cout << "\n=== CURVE DRAWING ===\n";
    std::cout << "Left mouse : Add point\n";
    std::cout << "Enter      : Generate curve\n";
    std::cout << "C          : Clear all points\n";
    std::cout << "Esc        : Exit\n\n";

    // Main rendering loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Draw the control polygon in gray
        if (points.size() >= 2)
        {
            drawPoints(
                points, GL_LINE_STRIP,
                0.45f, 0.45f, 0.50f
            );
        }

        // Draw the generated curve in green
        if (curveReady)
        {
            drawPoints(
                curve, GL_LINE_STRIP,
                0.10f, 1.00f, 0.35f
            );
        }

        // Draw control points as large red dots
        drawPoints(
            points, GL_POINTS,
            1.00f, 0.20f, 0.25f, 10.0f
        );

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}