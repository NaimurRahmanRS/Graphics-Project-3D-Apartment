// Naimur Rahman
// Roll: 1907031
// 3D Classroom Assignment

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"
#include "camera.h"
#include "pointLight.h"
#include "cube.h"
#include "stb_image.h"
#include "cylinder.h"
#include "sphere.h"
#include "cone.h"

#include <iostream>

using namespace std;

//function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawTableChair(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
void drawFan(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color);
void drawSofa(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
void drawTeaTable(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
void drawDinningTableChair(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L);
void read_file(string file_name, vector<float>& vec);
unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices, float div);
long long nCr(int n, int r);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//screen
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float aspectRatio = 4.0f / 3.0f;

//camera
float eyeX = 0.0f, eyeY = 5.0f, eyeZ = 5.0f;
float lookAtX = 2.0f, lookAtY = 1.5f, lookAtZ = 4.75f;
Camera camera(glm::vec3(eyeX, eyeY, eyeZ));

//timing
float deltaTime = 0.0f;     // time between current frame and last frame
float lastFrame = 0.0f;

//fan
float r = 0.0f;
bool fanOn = false;

//bird's eye view
bool birdEyeView = false;
glm::vec3 birdEyePosition(2.0f, 3.5f, 13.5f);
glm::vec3 birdEyeTarget(2.0f, 0.0f, 7.5f);
float birdEyeSpeed = 1.0f;

//rotation around a point
float theta = 0.0f; // Angle around the Y-axis
float radius = 2.0f;

//directional light
bool directionLightOn = true;
bool directionalAmbient = true;
bool directionalDiffuse = true;
bool directionalSpecular = true;

//spot light
bool spotLightOn = true;

//point light
bool point1 = true;
bool point2 = true;

//doors
bool openDoor = true;
float doorAngle = 90.0f;

//custom projection matrix
float fov = glm::radians(camera.Zoom);
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float near = 0.1f;
float far = 100.0f;
float tanHalfFOV = tan(fov / 2.0f);

//bezier
const double pi = 3.14159265389;
const int nt = 40;
const int ntheta = 30;

unsigned int sphereVAO, coneVAO;

vector <float> cntrlPointsSphere, cntrlPointsCone;
vector <float> coordinatesSphere, coordinatesCone;
vector <float> normalsSphere, normalsCone;
vector <int> indicesSphere, indicesCone;
vector <float> verticesSphere, verticesCone;


//positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(-4.1f,  3.5f,  0.1f),
    glm::vec3(-4.1f,  3.5f,  9.7f),
};

PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,       // position
    0.2f, 0.2f, 0.2f,       //ambient
    0.8f, 0.8f, 0.8f,       //diffuse
    0.2f, 0.2f, 0.2f,       //specular
    1.0f,       //k_c
    0.09f,      //k_l
    0.032f,     //k_q
    1       //light number
);

PointLight pointlight2(
    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,
    0.2f, 0.2f, 0.2f,
    0.8f, 0.8f, 0.8f,
    0.2f, 0.2f, 0.2f,
    1.0f,
    0.09f,
    0.032f,
    2
);

int main()
{
    //glfw initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "1907031: Assignment 3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //glad load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //configure global opengl state
    glEnable(GL_DEPTH_TEST);

    //build and compile our shader program
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    //Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    //set up vertex data (and buffer(s)) and configure vertex attributes
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    //note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //ourShader.use();
    //constantShader.use();

    read_file("sphere.txt", cntrlPointsSphere);
    sphereVAO = hollowBezier(cntrlPointsSphere.data(), ((unsigned int)cntrlPointsSphere.size() / 3) - 1, coordinatesSphere, normalsSphere, indicesSphere, verticesSphere, 2.0);

    //floor texture
    string diffuseMapPath = "floor.jpeg";
    string specularMapPath = "floor.jpeg";
    unsigned int diffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 10.0f, 15.0f);

    //garage texture
    string diffuseMapPath2 = "garage_floor.jpg";
    string specularMapPath2 = "garage_floor.jpg";
    unsigned int diffMap2 = loadTexture(diffuseMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap2 = loadTexture(specularMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube2 = Cube(diffMap2, specMap2, 32.0f, 0.0f, 0.0f, 3.5f, 2.0f);

    //bathroom texture
    string diffuseMapPath3 = "bathroom_floor.jpg";
    string specularMapPath3 = "bathroom_floor.jpg";
    unsigned int diffMap3 = loadTexture(diffuseMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap3 = loadTexture(specularMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube3 = Cube(diffMap3, specMap3, 32.0f, 0.0f, 0.0f, 3.0f, 4.0f);

    //bathroom texture
    string diffuseMapPath4 = "pathway.jpg";
    string specularMapPath4 = "pathway.jpg";
    unsigned int diffMap4 = loadTexture(diffuseMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap4 = loadTexture(specularMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube4 = Cube(diffMap4, specMap4, 32.0f, 0.0f, 0.0f, 5.0f, 2.0f);

    //ground texture
    string diffuseMapPath5 = "grass.jpg";
    string specularMapPath5 = "grass.jpg";
    unsigned int diffMap5 = loadTexture(diffuseMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap5 = loadTexture(specularMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube5 = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 5.0f, 6.0f);

    //ground texture
    string diffuseMapPath6 = "grass.jpg";
    string specularMapPath6 = "grass.jpg";
    unsigned int diffMap6 = loadTexture(diffuseMapPath6.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap6 = loadTexture(specularMapPath6.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube6 = Cube(diffMap6, specMap6, 32.0f, 0.0f, 0.0f, 5.0f, 7.0f);

    //basin texture
    string diffuseMapPath7 = "basin.jpg";
    string specularMapPath7 = "basin.jpg";
    unsigned int diffMap7 = loadTexture(diffuseMapPath7.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap7 = loadTexture(specularMapPath7.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    //sphere texture
    string diffuseMapPath8 = "basketball.jpg";
    string specularMapPath8 = "basketball.jpg";
    unsigned int diffMap8 = loadTexture(diffuseMapPath8.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap8 = loadTexture(specularMapPath8.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    //sphere texture
    string diffuseMapPath9 = "cone.jpg";
    string specularMapPath9 = "cone.jpg";
    unsigned int diffMap9 = loadTexture(diffuseMapPath9.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap9 = loadTexture(specularMapPath9.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    r = 0.0f;

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        //per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input
        processInput(window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //ourShader.use();
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        //point lights set up
        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);

        //directional light set up
        lightingShader.setVec3("directionalLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("directionalLight.specular", 0.0f, 0.0f, 0.0f);
        lightingShader.setBool("directionLightOn", directionLightOn);

        //spot light set up
        lightingShader.setVec3("spotLight.position", 2.0f, 2.0f, -3.0f);
        lightingShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("spotLight.specular", 0.2f, 0.2f, 0.2f);
        lightingShader.setFloat("spotLight.k_c", 1.0f);
        lightingShader.setFloat("spotLight.k_l", 0.09);
        lightingShader.setFloat("spotLight.k_q", 0.032);
        //lightingShader.setFloat("spotLight.cos_theta", glm::cos(glm::radians(30.0f)));
        lightingShader.setFloat("spotLight.inner_circle", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outer_circle", glm::cos(glm::radians(25.0f)));
        lightingShader.setBool("spotLightOn", spotLightOn);

        //handle for changes in directional light directly from shedder
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("ambientLight", !directionalAmbient);
                directionalAmbient = !directionalAmbient;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("diffuseLight", !directionalDiffuse);
                directionalDiffuse = !directionalDiffuse;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("specularLight", !directionalSpecular);
                directionalSpecular = !directionalSpecular;
            }
        }

        glm::mat4 projection(0.0f);
        projection[0][0] = 1.0f / (aspect * tanHalfFOV);
        projection[1][1] = 1.0f / tanHalfFOV;
        projection[2][2] = -(far + near) / (far - near);
        projection[2][3] = -1.0f;
        projection[3][2] = -(2.0f * far * near) / (far - near);
        //pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        //ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);
        lightingShader.setMat4("projection", projection);

        //camera view transformation
        //constantShader.setMat4("view", view);
        //ourShader.setMat4("view", view);

        glm::mat4 view;

        //check for bird's eye view and normal view
        if (birdEyeView) {
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = camera.GetViewMatrix();
        }
        
        lightingShader.setMat4("view", view);

        //define matrices and vectors needed
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
        glm::vec3 color;
        
        //initialize all elements as non-emissive
        lightingShader.setVec3("material.emissive", glm::vec3(0.0f, 0.0f, 0.0f));

        //draw tables and chairs
        float z = 0.0f;

        if (openDoor) {
            if (doorAngle < 90.0f) {
                doorAngle += 0.1;
            }
        }

        if (!openDoor) {
            if (doorAngle > 0.0f) {
                doorAngle -= 0.1;
            }
        }

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 3.0f, -19.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 6.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -13.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 3.0f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //out door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, 2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);

        //out door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, -2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left Wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 10.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //right wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 30.0f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //back wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.0f, 5.0f, 0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //front wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.0f, 5.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //garage wall fb
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 8.0f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //garage wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(14.0f, 5.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //bathroom wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, 3.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //bathroom door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 4.0f, -0.1f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);

        //bathroom wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 5.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //dinning wall fb
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 8.0f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //dinning wall fb
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 3.0f, 8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.0f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //dinning to kitchen door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -1.0f, 8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.0f, 2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);

        //dinning wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 5.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //dinning wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 3.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //dinning door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 4.0f, -0.1f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);

        //kitchen wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 3.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //kitchen door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 4.0f, -0.1f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);

        //kitchen wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(6.0f, 5.0f, -0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //lift wall fb
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 4.0f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //lift wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 5.0f, 0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //lift wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.5f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 5.0f, 0.1f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);


        float stair_y = 4.0f, stair_z = -12.0f;

        for (int i = 0; i < 10; i++) {
            //floor stair
            translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, stair_y, stair_z));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -0.1f, 0.5f));
            model = translateMatrix * scaleMatrix;
            color = glm::vec3(0.494f, 0.514f, 0.541f);
            drawCube(VAO, lightingShader, model, color);

            stair_z = stair_z + 0.5f;

            //wall stair
            translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, stair_y, stair_z + 0.1f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -0.5f, -0.1f));
            model = translateMatrix * scaleMatrix;
            color = glm::vec3(0.659f, 0.820f, 0.843f);
            drawCube(VAO, lightingShader, model, color);

            stair_y = stair_y - 0.5f;
        }

        //sofa
        drawSofa(VAO, lightingShader, identityMatrix);

        //TV
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 2.0f, 0.1f));
        color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        drawTeaTable(VAO, lightingShader, identityMatrix);

        drawDinningTableChair(VAO, lightingShader, identityMatrix);

        /*
        //light holder 1 with emissive material property
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.08f, 3.5f, 2.08f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.04f, -1.5f, 0.04f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.5f, 0.0f, 0.0f);

        lightingShader.setVec3("material.ambient", color);
        lightingShader.setVec3("material.diffuse", color);
        lightingShader.setVec3("material.specular", color);
        lightingShader.setVec3("material.emissive", color);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //light holder 2 with emissive material property
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.08f, 3.5f, 8.08f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.04f, -1.5f, 0.04f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.5f);

        lightingShader.setVec3("material.ambient", color);
        lightingShader.setVec3("material.diffuse", color);
        lightingShader.setVec3("material.specular", color);
        lightingShader.setVec3("material.emissive", color);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        */

        //draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);

        for (unsigned int i = 0; i < 2; i++)
        {
            translateMatrix = glm::translate(identityMatrix, pointLightPositions[i]);
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.2f, 0.2f));
            model = translateMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        
        //glfw swap buffers and poll IO events (keys pressed/released, mouse moved etc.)

        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setMat4("projection", projection);
        lightingShaderWithTexture.setMat4("view", view);

        //point lights set up
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        pointlight2.setUpPointLight(lightingShaderWithTexture);

        //directional light set up
        lightingShaderWithTexture.setVec3("directionalLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShaderWithTexture.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShaderWithTexture.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShaderWithTexture.setVec3("directionalLight.specular", 0.0f, 0.0f, 0.0f);
        lightingShaderWithTexture.setBool("directionLightOn", directionLightOn);

        //spot light set up
        lightingShaderWithTexture.setVec3("spotLight.position", 2.0f, 2.0f, -3.0f);
        lightingShaderWithTexture.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShaderWithTexture.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShaderWithTexture.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShaderWithTexture.setVec3("spotLight.specular", 0.2f, 0.2f, 0.2f);
        lightingShaderWithTexture.setFloat("spotLight.k_c", 1.0f);
        lightingShaderWithTexture.setFloat("spotLight.k_l", 0.09);
        lightingShaderWithTexture.setFloat("spotLight.k_q", 0.032);
        //lightingShaderWithTexture.setFloat("spotLight.cos_theta", glm::cos(glm::radians(30.0f)));
        lightingShaderWithTexture.setFloat("spotLight.inner_circle", glm::cos(glm::radians(12.5f)));
        lightingShaderWithTexture.setFloat("spotLight.outer_circle", glm::cos(glm::radians(25.0f)));
        lightingShaderWithTexture.setBool("spotLightOn", spotLightOn);

        //floor house
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(14.0f, -0.1f, 8.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor house
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(6.0f, -0.1f, 8.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor house
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.0f, -0.1f, 22.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor garden
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 4.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube4.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor garden
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 12.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube5.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor garden
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 14.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube6.drawCubeWithTexture(lightingShaderWithTexture, model);

        //basin using bezier curve
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.5, 1.0, 0.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateMatrix * rotateZMatrix * scaleMatrix;
        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", 32.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffMap7);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specMap7);
        glBindVertexArray(sphereVAO);
        lightingShaderWithTexture.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesSphere.size(), GL_UNSIGNED_INT, (void*)0);

        //pipe
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -1.15f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.56f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        Cylinder c = Cylinder(0.05f, 36, 10, color, color, color, 32.0f);
        c.drawSphereTexture(lightingShaderWithTexture, model, diffMap7, specMap7);

        //basketball
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, -0.6f, 2.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        Sphere s = Sphere(0.4f, 36, 36, color, color, color, 32.0f);
        s.drawSphereTexture(lightingShaderWithTexture, model, diffMap8, specMap8);

        //cone
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 1.0f, 6.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        Cone cone = Cone(0.4f, 1.0f, 36, color, color, color, 32.0f);
        cone.drawConeTexture(lightingShaderWithTexture, model, diffMap9, specMap9);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //glfw terminate, clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

void drawDinningTableChair(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
    glm::vec3 color;

    //top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.5f, 1.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(7.0f, 0.1f, 3.0f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec3(0.624f, 0.416f, 0.310f);
    drawCube(VAO, lightingShader, model, color);

    //back left leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.5f, 1.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -2.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //front left leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.5f, 1.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -2.0f, -0.1f));
    model = translateMatrix * scaleMatrix;
    //color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //back right leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 1.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -2.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    //color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //front right leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 1.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -2.0f, -0.1f));
    model = translateMatrix * scaleMatrix;
    //color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 9.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.0f, 9.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 9.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 9.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 9.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 9.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5f, 0.0f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.5f, 0.0f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.5f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.0f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.0f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.0f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.5f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);
}

void drawTeaTable(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
    glm::vec3 color;

    //top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 0.0f, -6.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, -2.0f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec3(0.624f, 0.416f, 0.310f);
    drawCube(VAO, lightingShader, model, color);

    //back left leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 0.0f, -6.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, -0.1f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //front left leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 0.0f, -8.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    //color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //back right leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.0f, -6.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, -0.1f));
    model = translateMatrix * scaleMatrix;
    //color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //front right leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.0f, -8.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    //color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);
}

void drawSofa(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
    glm::vec3 color;

    //top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.0f, -6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.0f, 2.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    drawCube(VAO, lightingShader, model, color);

    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.0f, -4.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 2.5f, 0.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.0f, -6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 1.5f, 2.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f, -1.0f, -6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 2.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f, -1.0f, -6.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -3.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    drawCube(VAO, lightingShader, model, color);

    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, -1.0f, -6.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 2.5f, -3.0f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);
    
    //left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f, -1.0f, -6.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 1.5f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f, -1.0f, -9.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5f, 1.5f, -0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);
}

void drawFan(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
    glm::vec3 color;

    //when fan is on
    if (fanOn) {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.28f, 3.5f, 1.61f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.75f, 0.1f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan middle
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.46f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.1f, 0.4f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.56f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.53f, 2.75f, 1.56f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.46f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, -0.75f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.86f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, -0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, 0.75f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        r += 1.0f;
    }

    //when fan is off
    else {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.28f, 3.5f, 1.61f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.75f, 0.1f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan middle
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.46f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.1f, 0.4f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.53f, 2.75f, 1.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.46f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, -0.75f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.86f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, 0.75f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
    }
}

void drawTableChair(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //table top
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    drawCube(VAO, lightingShader, model, color);

    //table back
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, -1.0f, 0.1f));
    model = matrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //table leg left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //table leg right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 0.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //table mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.75f, 0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 0.8f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    drawCube(VAO, lightingShader, model, color);

    //PC
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, -0.65f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.25f, 0.5f, 0.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //monitor base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4f, 0.1f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.05f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //monitor pipe
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.45f, 0.15f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //monitor screen
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.3f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //keyboard
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.15f, 0.1f, 0.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.05f, 0.25f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //mouse
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 0.1f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.05f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.1f, 0.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper piller left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.65f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper piller right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.65f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.15f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.1f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.20f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.1f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);
}

void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color)
{
    //use the shadder
    lightingShader.use();

    //define lighting properties
    lightingShader.setVec3("material.ambient", color);
    lightingShader.setVec3("material.diffuse", color);
    lightingShader.setVec3("material.specular", color);
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

//process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if(!birdEyeView)
            camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        fanOn = !fanOn;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        birdEyeView = !birdEyeView;
    }

    if (birdEyeView) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            birdEyePosition.z -= birdEyeSpeed * deltaTime; // Move forward along Z
            birdEyeTarget.z -= birdEyeSpeed * deltaTime;
            if (birdEyePosition.z <= 2.0) {
                birdEyePosition.z = 2.0;
            }
            if (birdEyeTarget.z <= -4.0) {
                birdEyeTarget.z = -4.0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            birdEyePosition.z += birdEyeSpeed * deltaTime; // Move backward along Z
            birdEyeTarget.z += birdEyeSpeed * deltaTime;
            if (birdEyePosition.z >= 13.5) {
                birdEyePosition.z = 13.5;
            }
            if (birdEyeTarget.z >= 7.5) {
                birdEyeTarget.z = 7.5;
            }
        }
    }

    /*
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {   
        if (!birdEyeView) {
            theta += 0.025f;
            camera.Position.x = lookAtX + radius * sin(theta);
            camera.Position.y = lookAtY;
            camera.Position.z = lookAtZ + radius * cos(theta);
        }
    }
    */

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (pointlight1.ambientOn > 0 && pointlight1.diffuseOn > 0 && pointlight1.specularOn > 0) {
            pointlight1.turnOff();
            point1 = false;
        }    
        else {
            pointlight1.turnOn();
            point1 = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (pointlight2.ambientOn > 0 && pointlight2.diffuseOn > 0 && pointlight2.specularOn > 0) {
            pointlight2.turnOff();
            point2 = false;
        }            
        else {
            pointlight2.turnOn();
            point2 = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        directionLightOn = !directionLightOn;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        spotLightOn = !spotLightOn;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (pointlight1.ambientOn > 0 || pointlight2.ambientOn > 0) {
            if(point1)
                pointlight1.turnAmbientOff();
            if(point2)
                pointlight2.turnAmbientOff();
        }
        else {
            if(point1)
                pointlight1.turnAmbientOn();
            if(point2)
                pointlight2.turnAmbientOn();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (pointlight1.diffuseOn > 0 || pointlight2.diffuseOn > 0) {
            if (point1)
                pointlight1.turnDiffuseOff();
            if (point2)
                pointlight2.turnDiffuseOff();
        }
        else {
            if (point1)
                pointlight1.turnDiffuseOn();
            if (point2)
                pointlight2.turnDiffuseOn();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (pointlight1.specularOn > 0 || pointlight2.specularOn > 0) {
            if (point1)
                pointlight1.turnSpecularOff();
            if (point2)
                pointlight2.turnSpecularOff();
        }
        else {
            if (point1)
                pointlight1.turnSpecularOn();
            if (point2)
                pointlight2.turnSpecularOn();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        openDoor = true;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        openDoor = false;
    }
}

//glfw whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //make sure the viewport matches the new window dimensions; note that width and height will be significantly larger than specified on retina displays.
    float aspectRatio = 4.0f / 3.0f;
    int viewWidth, viewHeight;

    if (width / (float)height > aspectRatio) {
        //Window is too wide, fit height and adjust width
        viewHeight = height;
        viewWidth = (int)(height * aspectRatio);
    }
    else {
        //Window is too tall, fit width and adjust height
        viewWidth = width;
        viewHeight = (int)(width / aspectRatio);
    }

    //Center the viewport
    int xOffset = (width - viewWidth) / 2;
    int yOffset = (height - viewHeight) / 2;

    glViewport(xOffset, yOffset, viewWidth, viewHeight);
    //glViewport(0, 0, width, height);
}

//glfw whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;       //reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//glfw whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

long long nCr(int n, int r)
{
    if (r > n / 2)
        r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for (i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
{
    double y = 0;
    double x = 0;
    t = t > 1.0 ? 1.0 : t;
    for (int i = 0; i < L + 1; i++)
    {
        long long ncr = nCr(L, i);
        double oneMinusTpow = pow(1 - t, double(L - i));
        double tPow = pow(t, double(i));
        double coef = oneMinusTpow * tPow * ncr;
        x += coef * ctrlpoints[i * 3];
        y += coef * ctrlpoints[(i * 3) + 1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices, float div = 1.0)
{
    int i, j;
    float x, y, z, r;                //current coordinates
    float theta;
    float nx, ny, nz, lengthInv;    // vertex normal

    const float dtheta = (2 * pi / ntheta) / div;        //angular step size

    float t = 0;
    float dt = 1.0 / nt;
    float xy[2];
    vector<float> textureCoords;

    for (i = 0; i <= nt; ++i)              //step through y
    {
        BezierCurve(t, xy, ctrlpoints, L);
        r = xy[0];
        y = xy[1];
        theta = 0;
        t += dt;
        lengthInv = 1.0 / r;

        for (j = 0; j <= ntheta; ++j)
        {
            // Calculate (x, y, z) coordinates using parametric equations
            double cosa = cos(theta);
            double sina = sin(theta);
            z = r * cosa;
            x = r * sina;

            coordinates.push_back(x);
            coordinates.push_back(y);
            coordinates.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            // center point of the circle (0,y,0)
            nx = -(x - 0) * lengthInv;
            ny = -(y - y) * lengthInv;
            nz = -(z - 0) * lengthInv;

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // Texture coordinates (u, v)
            float u = float(j) / float(ntheta); // Around the circle
            float v = float(i) / float(nt);     // Along the curve
            textureCoords.push_back(u);
            textureCoords.push_back(v);

            // Increment theta for next iteration
            theta += dtheta;
        }
    }

    // Generate index list of triangles
    int k1, k2;
    for (int i = 0; i < nt; ++i)
    {
        k1 = i * (ntheta + 1);     // beginning of current stack
        k2 = k1 + ntheta + 1;      // beginning of next stack

        for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
        {
            // Create two triangles for each segment
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    // Store the vertex data (positions, normals, texture coordinates)
    size_t count = coordinates.size();
    for (int i = 0; i < count; i += 3)
    {
        vertices.push_back(coordinates[i]);
        vertices.push_back(coordinates[i + 1]);
        vertices.push_back(coordinates[i + 2]);

        vertices.push_back(normals[i]);
        vertices.push_back(normals[i + 1]);
        vertices.push_back(normals[i + 2]);

        // Add texture coordinates
        vertices.push_back(textureCoords[i / 3 * 2]);
        vertices.push_back(textureCoords[i / 3 * 2 + 1]);
    }

    // Create VAO, VBO, and EBO for OpenGL rendering
    unsigned int bezierVAO;
    glGenVertexArrays(1, &bezierVAO);
    glBindVertexArray(bezierVAO);

    // Create VBO to copy vertex data to GPU
    unsigned int bezierVBO;
    glGenBuffers(1, &bezierVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // For vertex data
    glBufferData(GL_ARRAY_BUFFER,                   // Target
        (unsigned int)vertices.size() * sizeof(float), // Size in bytes
        vertices.data(),   // Pointer to vertex data
        GL_STATIC_DRAW);                   // Usage

    // Create EBO to copy index data to GPU
    unsigned int bezierEBO;
    glGenBuffers(1, &bezierEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // For index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // Target
        (unsigned int)indices.size() * sizeof(unsigned int),             // Size in bytes
        indices.data(),               // Pointer to index data
        GL_STATIC_DRAW);                   // Usage

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // Set attribute pointers
    int stride = 32;     // Should be 32 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);            // Vertex position
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3)); // Normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float))); // Texture Coord

    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return bezierVAO;
}


void read_file(string file_name, vector<float>& vec)
{
    ifstream file(file_name);
    float number;

    while (file >> number)
        vec.push_back(number);

    file.close();
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}