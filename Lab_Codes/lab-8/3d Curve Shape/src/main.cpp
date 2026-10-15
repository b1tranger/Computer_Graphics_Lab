
#include "glad.h"
#include "glfw3.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int WIDTH = 1000;
const int HEIGHT = 700;
const float PI = 3.14159265359f;
const int SLICES = 64;
const int STEPS = 12;

struct Point {
    float r, y;
};

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
};

struct Vec3 {
    float x, y, z;
};

struct Mat4 {
    float m[16];
};

std::vector<Point> profile, smoothProfile;
std::vector<Vertex> mesh, editorVertices;

unsigned int VAO, VBO, program;
int modelLoc, viewLoc, projLoc, colorLoc, lightLoc;

bool bottleReady = false;
bool wireframe = false;
bool dragging = false;

float yaw = 0.65f;
float pitch = 0.2f;
float cameraDistance = 4.5f;

double lastX = 0, lastY = 0;

const float AXIS_X = -0.55f;
const float PROFILE_SCALE = 0.42f;

// ---------------- SHADERS ----------------

const char* vertexShaderSource = R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 WorldPos;

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    WorldPos = world.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * world;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 Normal;
in vec3 WorldPos;

uniform vec3 uColor;
uniform int uLighting;

out vec4 FragColor;

void main()
{
    if (uLighting == 0) {
        FragColor = vec4(uColor, 1.0);
        return;
    }

    vec3 N = normalize(Normal);
    vec3 L = normalize(vec3(-0.6, 1.0, 0.8));
    vec3 V = normalize(vec3(2.0, 3.0, 4.0) - WorldPos);
    vec3 H = normalize(L + V);

    float diffuse = max(dot(N, L), 0.0);
    float specular = pow(max(dot(N, H), 0.0), 48.0);

    vec3 result =
        0.22 * uColor +
        0.75 * diffuse * uColor +
        0.35 * specular * vec3(1.0);

    FragColor = vec4(result, 1.0);
}
)";

// ---------------- VECTOR MATH ----------------

Vec3 sub(Vec3 a, Vec3 b) {
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}

Vec3 cross(Vec3 a, Vec3 b) {
    return {
        a.y*b.z-a.z*b.y,
        a.z*b.x-a.x*b.z,
        a.x*b.y-a.y*b.x
    };
}

float dot(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 normalize(Vec3 v) {
    float len = std::sqrt(dot(v,v));
    if (len < 0.00001f) return {0,1,0};
    return {v.x/len,v.y/len,v.z/len};
}

// ---------------- MATRICES ----------------

Mat4 identity() {
    Mat4 a = {};
    a.m[0]=a.m[5]=a.m[10]=a.m[15]=1.0f;
    return a;
}

Mat4 perspective(float fov, float aspect,
                 float nearP, float farP) {
    Mat4 a = {};
    float f = 1.0f/std::tan(fov*0.5f);
    a.m[0]=f/aspect;
    a.m[5]=f;
    a.m[10]=(farP+nearP)/(nearP-farP);
    a.m[11]=-1.0f;
    a.m[14]=(2.0f*farP*nearP)/(nearP-farP);
    return a;
}

Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = normalize(sub(target,eye));
    Vec3 s = normalize(cross(f,up));
    Vec3 u = cross(s,f);

    Mat4 a=identity();

    a.m[0]=s.x; a.m[4]=s.y; a.m[8]=s.z;
    a.m[1]=u.x; a.m[5]=u.y; a.m[9]=u.z;
    a.m[2]=-f.x; a.m[6]=-f.y; a.m[10]=-f.z;

    a.m[12]=-dot(s,eye);
    a.m[13]=-dot(u,eye);
    a.m[14]=dot(f,eye);

    return a;
}

// ---------------- SHADER SETUP ----------------

unsigned int compileShader(GLenum type, const char* src) {
    unsigned int s=glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);

    int ok;
    char log[512];
    glGetShaderiv(s,GL_COMPILE_STATUS,&ok);

    if(!ok) {
        glGetShaderInfoLog(s,512,nullptr,log);
        std::cout << "Shader error:\n" << log << '\n';
    }
    return s;
}

unsigned int createProgram() {
    unsigned int vs=compileShader(GL_VERTEX_SHADER,vertexShaderSource);
    unsigned int fs=compileShader(GL_FRAGMENT_SHADER,fragmentShaderSource);
    unsigned int p=glCreateProgram();

    glAttachShader(p,vs);
    glAttachShader(p,fs);
    glLinkProgram(p);

    int ok;
    char log[512];
    glGetProgramiv(p,GL_LINK_STATUS,&ok);

    if(!ok) {
        glGetProgramInfoLog(p,512,nullptr,log);
        std::cout << "Link error:\n" << log << '\n';
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return p;
}

// ---------------- CURVE ----------------

Point catmullRom(Point a, Point b, Point c, Point d, float t) {
    float t2=t*t, t3=t2*t;

    Point p;
    p.r=0.5f*(2*b.r+(-a.r+c.r)*t+
        (2*a.r-5*b.r+4*c.r-d.r)*t2+
        (-a.r+3*b.r-3*c.r+d.r)*t3);

    p.y=0.5f*(2*b.y+(-a.y+c.y)*t+
        (2*a.y-5*b.y+4*c.y-d.y)*t2+
        (-a.y+3*b.y-3*c.y+d.y)*t3);

    p.r=std::max(0.015f,p.r);
    return p;
}

void makeSmoothProfile() {
    smoothProfile.clear();

    for(size_t i=0;i+1<profile.size();i++) {
        Point a=(i==0)?profile[i]:profile[i-1];
        Point b=profile[i];
        Point c=profile[i+1];
        Point d=(i+2<profile.size())?profile[i+2]:profile[i+1];

        for(int j=0;j<STEPS;j++) {
            smoothProfile.push_back(
                catmullRom(a,b,c,d,float(j)/STEPS));
        }
    }

    smoothProfile.push_back(profile.back());
}

Vertex makeVertex(Vec3 p, Vec3 n) {
    n=normalize(n);
    return {p.x,p.y,p.z,n.x,n.y,n.z};
}

void addTriangle(Vertex a, Vertex b, Vertex c) {
    mesh.push_back(a);
    mesh.push_back(b);
    mesh.push_back(c);
}

// ---------------- BOTTLE MESH ----------------

void generateBottle() {
    if(profile.size()<3) {
        std::cout << "Click at least 3 points first.\n";
        return;
    }

    for(size_t i=1;i<profile.size();i++) {
        if(profile[i].y<=profile[i-1].y) {
            std::cout << "Draw from bottom to top.\n";
            return;
        }
    }

    mesh.clear();
    makeSmoothProfile();

    int count=(int)smoothProfile.size();
    std::vector<Vertex> rings(count*(SLICES+1));

    for(int i=0;i<count;i++) {
        Point p=smoothProfile[i];
        Point before=smoothProfile[std::max(0,i-1)];
        Point after=smoothProfile[std::min(count-1,i+1)];

        float dr=after.r-before.r;
        float dy=after.y-before.y;

        for(int j=0;j<=SLICES;j++) {
            float a=2*PI*j/SLICES;
            float c=std::cos(a), s=std::sin(a);

            Vec3 pos={p.r*c,p.y,p.r*s};
            Vec3 n={dy*c,-dr,dy*s};

            rings[i*(SLICES+1)+j]=makeVertex(pos,n);
        }
    }

    for(int i=0;i<count-1;i++) {
        for(int j=0;j<SLICES;j++) {
            Vertex a=rings[i*(SLICES+1)+j];
            Vertex b=rings[i*(SLICES+1)+j+1];
            Vertex c=rings[(i+1)*(SLICES+1)+j+1];
            Vertex d=rings[(i+1)*(SLICES+1)+j];

            addTriangle(a,b,c);
            addTriangle(a,c,d);
        }
    }

    // Flat bottom cap
    Point bottom=smoothProfile.front();
    Vec3 center={0,bottom.y,0};
    Vec3 n={0,-1,0};

    for(int j=0;j<SLICES;j++) {
        float a=2*PI*j/SLICES;
        float b=2*PI*(j+1)/SLICES;

        Vec3 p0={bottom.r*std::cos(a),bottom.y,bottom.r*std::sin(a)};
        Vec3 p1={bottom.r*std::cos(b),bottom.y,bottom.r*std::sin(b)};

        addTriangle(makeVertex(center,n),
                    makeVertex(p1,n),
                    makeVertex(p0,n));
    }

    bottleReady=true;
    std::cout << "Bottle generated. Triangles: "
              << mesh.size()/3 << '\n';
}

// ---------------- DRAWING ----------------

void uploadAndDraw(const std::vector<Vertex>& data,
                   GLenum mode,float r,float g,float b,
                   bool lighting,float size=1.0f) {
    if(data.empty()) return;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER,
        data.size()*sizeof(Vertex),data.data(),GL_DYNAMIC_DRAW);

    Mat4 I=identity();
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,I.m);
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,I.m);
    glUniformMatrix4fv(projLoc,1,GL_FALSE,I.m);
    glUniform3f(colorLoc,r,g,b);
    glUniform1i(lightLoc,lighting?1:0);
    glPointSize(size);

    glDrawArrays(mode,0,(GLsizei)data.size());
}

void drawEditor() {
    editorVertices.clear();

    editorVertices.push_back({AXIS_X,-0.9f,0,0,0,1});
    editorVertices.push_back({AXIS_X,0.9f,0,0,0,1});

    uploadAndDraw(editorVertices,GL_LINES,
                  0.4f,0.4f,0.45f,false);

    editorVertices.clear();

    for(Point p:profile) {
        editorVertices.push_back({
            AXIS_X+p.r*PROFILE_SCALE,p.y,0,0,0,1});
    }

    uploadAndDraw(editorVertices,GL_LINE_STRIP,
                  0.2f,0.85f,1.0f,false);

    uploadAndDraw(editorVertices,GL_POINTS,
                  1.0f,0.25f,0.2f,false,10.0f);
}

void drawBottle(GLFWwindow* window) {
    int w,h;
    glfwGetFramebufferSize(window,&w,&h);
    if(w<=0||h<=0) return;

    Vec3 eye={
        cameraDistance*std::cos(pitch)*std::sin(yaw),
        cameraDistance*std::sin(pitch),
        cameraDistance*std::cos(pitch)*std::cos(yaw)
    };

    Mat4 model=identity();
    Mat4 view=lookAt(eye,{0,0,0},{0,1,0});
    Mat4 proj=perspective(45*PI/180,float(w)/h,0.1f,100);

    glUniformMatrix4fv(modelLoc,1,GL_FALSE,model.m);
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,view.m);
    glUniformMatrix4fv(projLoc,1,GL_FALSE,proj.m);

    glUniform3f(colorLoc,0.15f,0.65f,0.9f);
    glUniform1i(lightLoc,1);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,mesh.size()*sizeof(Vertex),
                 mesh.data(),GL_DYNAMIC_DRAW);

    if(wireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glDrawArrays(GL_TRIANGLES,0,(GLsizei)mesh.size());

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

// ---------------- MOUSE AND KEYBOARD ----------------

void mouseButton(GLFWwindow* window,int button,int action,int mods) {
    double x,y;
    glfwGetCursorPos(window,&x,&y);

    int w,h;
    glfwGetWindowSize(window,&w,&h);
    if(w<=0||h<=0) return;

    if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_PRESS) {
        if(bottleReady) {
            dragging=true;
            lastX=x;
            lastY=y;
            return;
        }

        float nx=2*float(x)/w-1;
        float ny=1-2*float(y)/h;
        float radius=(nx-AXIS_X)/PROFILE_SCALE;

        if(radius<=0.02f) {
            std::cout << "Click right of the vertical axis.\n";
            return;
        }

        radius=std::min(radius,1.0f);

        if(!profile.empty()&&ny<=profile.back().y) {
            std::cout << "Click progressively higher points.\n";
            return;
        }

        profile.push_back({radius,ny});
        std::cout << "Point added: " << radius << ", " << ny << '\n';
    }

    if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_RELEASE)
        dragging=false;

    if(button==GLFW_MOUSE_BUTTON_RIGHT&&action==GLFW_PRESS&&!bottleReady) {
        if(!profile.empty()) profile.pop_back();
    }
}

void cursorPosition(GLFWwindow* window,double x,double y) {
    if(!dragging||!bottleReady) return;

    yaw+=float(x-lastX)*0.008f;
    pitch-=float(y-lastY)*0.008f;
    pitch=std::max(-1.45f,std::min(1.45f,pitch));

    lastX=x;
    lastY=y;
}

void scroll(GLFWwindow* window,double x,double y) {
    if(!bottleReady) return;
    cameraDistance-=float(y)*0.3f;
    cameraDistance=std::max(1.5f,std::min(10.0f,cameraDistance));
}

void keyCallback(GLFWwindow* window,int key,int sc,int action,int mods) {
    if(action!=GLFW_PRESS) return;

    if(key==GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window,true);

    if((key==GLFW_KEY_ENTER||key==GLFW_KEY_KP_ENTER)&&!bottleReady)
        generateBottle();

    if(key==GLFW_KEY_C) {
        profile.clear();
        smoothProfile.clear();
        mesh.clear();
        bottleReady=false;
        wireframe=false;
        dragging=false;
        std::cout << "Cleared. Draw a new profile.\n";
    }

    if(key==GLFW_KEY_W&&bottleReady)
        wireframe=!wireframe;
}

void framebufferSize(GLFWwindow* window,int w,int h) {
    glViewport(0,0,w,h);
}

// ---------------- MAIN ----------------

int main() {
    if(!glfwInit()) {
        std::cout << "GLFW initialization failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

    GLFWwindow* window=glfwCreateWindow(
        WIDTH,HEIGHT,"3D Bottle Modeller",nullptr,nullptr);

    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebufferSize);
    glfwSetMouseButtonCallback(window,mouseButton);
    glfwSetCursorPosCallback(window,cursorPosition);
    glfwSetScrollCallback(window,scroll);
    glfwSetKeyCallback(window,keyCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD initialization failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    program=createProgram();
    glUseProgram(program);

    modelLoc=glGetUniformLocation(program,"model");
    viewLoc=glGetUniformLocation(program,"view");
    projLoc=glGetUniformLocation(program,"projection");
    colorLoc=glGetUniformLocation(program,"uColor");
    lightLoc=glGetUniformLocation(program,"uLighting");

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,
                          sizeof(Vertex),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,
                          sizeof(Vertex),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    std::cout
        << "\n3D BOTTLE MODELLER\n"
        << "Left click : Add profile point\n"
        << "Right click: Undo last point\n"
        << "Enter      : Generate bottle\n"
        << "Drag mouse : Rotate camera\n"
        << "Scroll     : Zoom\n"
        << "W          : Wireframe toggle\n"
        << "C          : Clear and restart\n"
        << "Esc        : Exit\n"
        << "Draw bottom to top, right of the axis.\n\n";

    while(!glfwWindowShouldClose(window)) {
        int w,h;
        glfwGetFramebufferSize(window,&w,&h);
        glViewport(0,0,w,h);

        glClearColor(0.07f,0.08f,0.12f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        if(bottleReady)
            drawBottle(window);
        else
            drawEditor();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}