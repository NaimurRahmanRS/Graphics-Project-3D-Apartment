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
#include "cylinder_base.h"
#include "sphere.h"
#include "cone.h"
#include "triangle.h"

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
float eyeX = -15.0f, eyeY = 2.0f, eyeZ = -5.0f;
float lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;
Camera camera(glm::vec3(eyeX, eyeY, eyeZ));

//timing
float deltaTime = 0.0f;     // time between current frame and last frame
float lastFrame = 0.0f;

//fan
float r = 0.0f;
bool fanOn = false;

//bird's eye view
bool birdEyeView = false;
glm::vec3 birdEyePosition(0.0f, 50.0f, 0.0f);
glm::vec3 birdEyeTarget(0.0f, 1.0f, 1.0f);
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

//garage door
bool garageDoor = true;
float garageS = 0.0f;

//roof
float roofS = 0.0f;

//doors
bool openDoor = true;
float doorAngle = 90.0f;
float doorS = 0.0f;

//custom projection matrix
float fov = glm::radians(camera.Zoom);
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float near = 0.1f;
float far = 100.0f;
float tanHalfFOV = tan(fov / 2.0f);

//car
bool carOn = false;
float carTx = 0.0f;
float carTz = 0.0f;
float carR = 0.0f;
bool carUp = true;
float wheelR = 0.0f;

//swing
float swingAngle = 0.0f;
float varSwingAngle = 0.0f;
bool swingForward = true;

// Lift properties
bool liftUp = false;
float liftY = 0.0f;
bool doorL1 = true;
bool doorL2 = false;
float liftDB = 0.0f;
float liftDU = 0.0f;

//bezier
const double pi = 3.14159265389;
const int nt = 40;
const int ntheta = 30;

unsigned int sphereVAO, coneVAO, basinVAO;

vector <float> cntrlPointsSphere, cntrlPointsCone, cntrlPointsBasin;
vector <float> coordinatesSphere, coordinatesCone, coordinatesBasin;
vector <float> normalsSphere, normalsCone, normalsBasin;
vector <int> indicesSphere, indicesCone, indicesBasin;
vector <float> verticesSphere, verticesCone, verticesBasin;


//positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(-4.1f,  3.5f,  -12.0f),
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

    read_file("basin.txt", cntrlPointsBasin);
    basinVAO = hollowBezier(cntrlPointsBasin.data(), ((unsigned int)cntrlPointsBasin.size() / 3) - 1, coordinatesBasin, normalsBasin, indicesBasin, verticesBasin, 2.0);

    //floor texture
    string diffuseMapPath = "floor.jpg";
    string specularMapPath = "floor.jpg";
    unsigned int diffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 15.0f, 10.0f);
    Cube cube2 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 7.0f, 4.0f);

    //bathroom texture
    string diffuseMapPath3 = "bathroom_floor.jpg";
    string specularMapPath3 = "bathroom_floor.jpg";
    unsigned int diffMap3 = loadTexture(diffuseMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap3 = loadTexture(specularMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube3 = Cube(diffMap3, specMap3, 32.0f, 0.0f, 0.0f, 3.0f, 4.0f);

    //pathway texture
    string diffuseMapPath4 = "pathway.jpg";
    string specularMapPath4 = "pathway.jpg";
    unsigned int diffMap4 = loadTexture(diffuseMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap4 = loadTexture(specularMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube4 = Cube(diffMap4, specMap4, 32.0f, 0.0f, 0.0f, 5.0f, 2.0f);

    //grass texture
    string diffuseMapPath5 = "grass.jpg";
    string specularMapPath5 = "grass.jpg";
    unsigned int diffMap5 = loadTexture(diffuseMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap5 = loadTexture(specularMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube5 = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 5.0f, 6.0f);
    Cube cube6 = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 5.0f, 7.0f);

    //basin texture
    string diffuseMapPath7 = "basin.jpg";
    string specularMapPath7 = "basin.jpg";
    unsigned int diffMap7 = loadTexture(diffuseMapPath7.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap7 = loadTexture(specularMapPath7.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    //bathtub texture
    string diffuseMapPath8 = "bathtub.jpg";
    string specularMapPath8 = "bathtub.jpg";
    unsigned int diffMap8 = loadTexture(diffuseMapPath8.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap8 = loadTexture(specularMapPath8.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    //inner wall texture
    string diffuseMapPath9 = "wall_p.png";
    string specularMapPath9 = "wall_p.png";
    unsigned int diffMap9 = loadTexture(diffuseMapPath9.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap9 = loadTexture(specularMapPath9.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube9 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 10.0f, 2.5f);

    //stairs texture
    string diffuseMapPath10 = "stairs.jpg";
    string specularMapPath10 = "stairs.jpg";
    unsigned int diffMap10 = loadTexture(diffuseMapPath10.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap10 = loadTexture(specularMapPath10.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube10 = Cube(diffMap10, specMap10, 32.0f, 0.0f, 0.0f, 2.0f, 0.5f);

    //doors texture
    string diffuseMapPath11 = "door.jpg";
    string specularMapPath11 = "door.jpg";
    unsigned int diffMap11 = loadTexture(diffuseMapPath11.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap11 = loadTexture(specularMapPath11.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube11 = Cube(diffMap11, specMap11, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //tv texture
    string diffuseMapPath12 = "tv.jpg";
    string specularMapPath12 = "tv.jpg";
    unsigned int diffMap12 = loadTexture(diffuseMapPath12.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap12 = loadTexture(specularMapPath12.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube12 = Cube(diffMap12, specMap12, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //walls texture
    string diffuseMapPath13 = "road.jpg";
    string specularMapPath13 = "road.jpg";
    unsigned int diffMap13 = loadTexture(diffuseMapPath13.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap13 = loadTexture(specularMapPath13.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube13 = Cube(diffMap13, specMap13, 32.0f, 0.0f, 0.0f, 5.0f, 25.0f);
    Cube cube14 = Cube(diffMap13, specMap13, 32.0f, 0.0f, 0.0f, 15.0f, 5.0f);

    //kitchen texture
    string diffuseMapPath15 = "cabinet.jpg";
    string specularMapPath15 = "cabinet.jpg";
    unsigned int diffMap15 = loadTexture(diffuseMapPath15.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap15 = loadTexture(specularMapPath15.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube15 = Cube(diffMap15, specMap15, 32.0f, 0.0f, 0.0f, 5.0f, 1.0f);
    Cube cube16 = Cube(diffMap15, specMap15, 32.0f, 0.0f, 0.0f, 5.55f, 1.0f);
    Cube cube17 = Cube(diffMap15, specMap15, 32.0f, 0.0f, 0.0f, 2.0f, 1.0f);
    Cube cube18 = Cube(diffMap15, specMap15, 32.0f, 0.0f, 0.0f, 2.5f, 1.0f);

    //outer wall texture
    string diffuseMapPath19 = "outer_wall.jpg";
    string specularMapPath19 = "outer_wall.jpg";
    unsigned int diffMap19 = loadTexture(diffuseMapPath19.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap19 = loadTexture(specularMapPath19.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube19 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 10.1f, 3.5f);

    Cube cube20 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 15.0f, 2.5f);
    Cube cube21 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 15.0f, 3.5f);

    Cube cube22 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 0.5f, 2.5f);
    Cube cube23 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 0.5f, 3.5f);

    Cube cube24 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 3.0f, 0.5f);
    Cube cube25 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 3.0f, 1.5f);

    Cube cube26 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 2.0f, 2.5f);
    Cube cube27 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 2.0f, 3.5f);

    Cube cube28 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 2.0f, 0.5f);
    Cube cube29 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 2.0f, 1.5f);

    Cube cube30 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 7.0f, 2.5f);
    Cube cube31 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 7.0f, 3.5f);

    Cube cube32 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 4.0f, 2.5f);

    Cube cube33 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 1.0f, 0.5f);

    Cube cube34 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 5.0f, 2.5f);

    Cube cube35 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 3.0f, 2.5f);

    //roof texture
    string diffuseMapPath36 = "roof.jpg";
    string specularMapPath36 = "roof.jpg";
    unsigned int diffMap36 = loadTexture(diffuseMapPath36.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap36 = loadTexture(specularMapPath36.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube36 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 11.0f, 4.5f);

    Cube cube37 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 10.0f, 4.0f);

    Cube cube38 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 8.5f, 11.5f);
    Cube cube39 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 8.0f, 11.0f);

    Cube cube40 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 2.5f, 5.5f);
    Cube cube41 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 2.0f, 5.0f);

    Cube cube42 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 2.5f, 6.5f);
    Cube cube43 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 2.0f, 1.5f);

    Cube cube44 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 0.5f, 6.0f);
    //Cube cube45 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 0.55f, 6.0f);

    Cube cube46 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 0.5f, 5.0f);
    Cube cube47 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 2.0f, 5.0f);
    Cube cube48 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 2.0f, 2.5f);

    Cube cube49 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 1.0f, 0.5f);

    Cube cube50 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 5.05f, 2.5f);

    Cube cube51 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 4.25f, 2.5f);
    Cube cube52 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 1.5f, 0.5f);

    Cube cube53 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 4.25f, 2.5f);
    Cube cube54 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 1.5f, 0.5f);

    Cube cube55 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 3.5f, 2.5f);

    Cube cube56 = Cube(diffMap19, specMap19, 32.0f, 0.0f, 0.0f, 10.1f, 1.5f);
    
    Cube cube57 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 5.5f, 11.0f);
    Cube cube58 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 2.5f, 2.0f);
    Cube cube59 = Cube(diffMap36, specMap36, 32.0f, 0.0f, 0.0f, 0.5f, 2.5f);

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
        lightingShader.setVec3("directionalLight.diffuse", 0.6f, 0.6f, 0.6f);
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
            glm::vec3 up(0.0f, -1.0f, 0.0f);
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = camera.GetViewMatrix();
        }
        
        lightingShader.setMat4("view", view);

        //define matrices and vectors needed
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix, AlignTranslateMatrix, CarRotateYMatrix, AlignTranslateMatrix2;
        glm::vec3 color;
        
        //initialize all elements as non-emissive
        lightingShader.setVec3("material.emissive", glm::vec3(0.0f, 0.0f, 0.0f));

        //draw tables and chairs
        float z = 0.0f;

        if (openDoor) {
            if (doorAngle < 90.0f) {
                doorAngle += 0.5f;
            }
            if (doorS > 0.0f) {
                doorS -= 0.025f;
            }
            if (roofS < 0.0f) {
                roofS += 0.025f;
            }
        }

        if (!openDoor) {
            if (doorAngle > 0.0f) {
                doorAngle -= 0.5f;
            }
            if (doorS < 1.5f) {
                doorS += 0.025f;
            }
            if (roofS > -4.0f) {
                roofS -= 0.025;
            }
        }

        //swing 1
        color = glm::vec3(0.95f, 0.95f, 0.95f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.0f, 4.0f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 4.0f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.9f, 8.0f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, -0.1f, 0.1f));
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(swingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.45f, 8.0f, 7.05f));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.05f, 0.0f, -0.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -2.5f, 0.1f));
        model = translateMatrix * rotateXMatrix * AlignTranslateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.55f, 8.0f, 7.05f));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.05f, 0.0f, -0.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -2.5f, 0.1f));
        model = translateMatrix * rotateXMatrix * AlignTranslateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.0f, 8.0f, 7.05f));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, -2.5f, -1.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, -0.1f, 2.1f));
        model = translateMatrix * rotateXMatrix * AlignTranslateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //swing 2
        color = glm::vec3(0.95f, 0.95f, 0.95f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.0f, 4.0f, -17.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 4.0f, -17.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.0f, 0.1f));
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.9f, 8.0f, -17.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, -0.1f, 0.1f));
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(-swingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.45f, 8.0f, -16.95f));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.05f, 0.0f, -0.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -2.5f, 0.1f));
        model = translateMatrix * rotateXMatrix * AlignTranslateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.55f, 8.0f, -16.95f));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.05f, 0.0f, -0.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -2.5f, 0.1f));
        model = translateMatrix * rotateXMatrix * AlignTranslateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.0f, 8.0f, -16.95f));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, -2.5f, -1.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, -0.1f, 2.1f));
        model = translateMatrix * rotateXMatrix * AlignTranslateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        if (varSwingAngle) {
            if (swingAngle >= varSwingAngle) {
                varSwingAngle -= 2.0f;
                swingAngle = varSwingAngle;
                swingForward = false;
            }
            if (swingAngle <= -varSwingAngle) {
                varSwingAngle -= 2.0f;
                swingAngle = -varSwingAngle;
                swingForward = true;
            }
            if (swingForward) {
                swingAngle += 1.0f;
            }
            else {
                swingAngle -= 1.0f;
            }
            if (varSwingAngle < 0.0f) {
                varSwingAngle = 0.0f;
                if (varSwingAngle == 0.0f) {
                    swingAngle = 0.0f;
                }
            }
        }

        //sofa
        drawSofa(VAO, lightingShader, identityMatrix);

        drawTeaTable(VAO, lightingShader, identityMatrix);

        drawDinningTableChair(VAO, lightingShader, identityMatrix);

        if (!carOn) {
            //car
            color = glm::vec3(0.95f, 0.95f, 0.95f);
            wheelR = 0.0f;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, -16.0f));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-4.5f, -0.75f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(9.0f, 1.25f, 4.0f));
            model = translateMatrix * AlignTranslateMatrix * scaleMatrix;
            drawCube(VAO, lightingShader, model, color);

            color = glm::vec3(0.25f, 0.25f, 0.25f);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, -16.0f));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.5f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 1.5f, 4.0f));
            model = translateMatrix * AlignTranslateMatrix * scaleMatrix;
            drawCube(VAO, lightingShader, model, color);

            Triangle trianlge = Triangle(color, color, color, 32.0f);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, -16.0f));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.5f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, 4.0f));
            model = translateMatrix * AlignTranslateMatrix * scaleMatrix;
            trianlge.drawTriangleWithMaterialisticProperty(lightingShader, model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, -16.0f));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.5f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, 4.0f));
            model = translateMatrix * rotateYMatrix * AlignTranslateMatrix * scaleMatrix;
            trianlge.drawTriangleWithMaterialisticProperty(lightingShader, model);
        }

        CarRotateYMatrix = glm::rotate(identityMatrix, glm::radians(carR), glm::vec3(0.0f, 1.0f, 0.0f));

        if (carOn) {
            color = glm::vec3(0.95f, 0.95f, 0.95f);           
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-4.5f, -0.75f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(9.0f, 1.25f, 4.0f));
            model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * scaleMatrix;
            drawCube(VAO, lightingShader, model, color);

            color = glm::vec3(0.25f, 0.25f, 0.25f);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.5f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 1.5f, 4.0f));
            model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * scaleMatrix;
            drawCube(VAO, lightingShader, model, color);

            Triangle trianlge = Triangle(color, color, color, 32.0f);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.5f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, 4.0f));
            model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * scaleMatrix;
            trianlge.drawTriangleWithMaterialisticProperty(lightingShader, model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
            AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.5f, -2.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.5f, 4.0f));
            model = translateMatrix * CarRotateYMatrix * rotateYMatrix * AlignTranslateMatrix * scaleMatrix;
            trianlge.drawTriangleWithMaterialisticProperty(lightingShader, model);

            wheelR += 1.0f;
            
            if (carTx == -22.0f && carTz < 4.9f && carUp == true) {
                //cond11
                carTz = carTz + 0.1f;
                //cout << "cond11" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTz > 4.9f) {
                    carTz = 4.9f;
                    carUp = false;
                }
            }

            else if (carTx > -17.0f && carTz == 0.0f && carUp == true) {
                //cond1
                carTx = carTx - 0.1f;
                //cout << "cond1" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx < -17.0f) {
                    carTx = -17.0f;
                }
            }

            else if (carTx > -22.0f && carTz < 5.0f && carTx <= -17.0f && carTz >= 0.0f && carUp == true) {
                //cond2
                carTx = carTx - 0.02f;
                carR = carR + 0.36f;
                carTz = carTz + 0.02f;
                //cout << "cond2" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx < -22.0f) {
                    carTx = -22.0f;
                }
                if (carTz > 5.0f) {
                    carTz = 5.0f;
                }
            }

            else if (carTz < 26.0f && carTz > 4.99f && carTx == -22.0f && carUp == true) {
                //cond3
                carTz = carTz + 0.1f;
                //cout << "cond3" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTz > 26.0f) {
                    carTz = 26.0f;
                }
            }

            else if (carTz < 31.0f && carTx < -17.0f && carTz >= 26.0f && carTx >= -22.0f && carUp == true) {
                //cond4
                carTx = carTx + 0.05f;
                carR = carR + 0.9f;
                carTz = carTz + 0.05f;
                //cout << "cond4" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx > -17.0f) {
                    carTx = -17.0f;
                }
                if (carTz > 31.0f) {
                    carTz = 31.0f;
                }
            }
          
            else if (carTx < 13.0f && carTz == 31.0f && carUp == true) {
                //cond5
                carTx = carTx + 0.1f;
                //cout << "cond5" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx > 13.0f) {
                    carTx = 13.0f;
                }
            }
            
            else if (carTx < 18.0f && carTz > 26.0f && carTx >= 13.0f && carTz <= 31.0f && carUp == true) {
                //cond6
                carTx = carTx + 0.05f;
                carR = carR + 0.9f;
                carTz = carTz - 0.05f;
                //cout << "cond6" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx > 18.0f) {
                    carTx = 18.0f;
                }
                if (carTz < 26.0f) {
                    carTz = 26.0f;
                }
            }

            else if (carTz > -4.0f && carTx == 18.0f && carUp == true) {
                //cond7
                carTz = carTz - 0.1f;
                //cout << "cond7" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTz < -4.0f) {
                    carTz = -4.0f;
                }
            }

            else if (carTx > 13.0f && carTz > -9.0f && carTx <= 18.0f && carTz <= -4.0f && carUp == true) {
                //cond8
                carTx = carTx - 0.05f;
                carR = carR + 0.9f;
                carTz = carTz - 0.05f;
                //cout << "cond8" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx < 13.0f) {
                    carTx = 13.0f;
                }
                if (carTz < -9.0f) {
                    carTz = -9.0f;
                }
            }

            else if (carTx > -17.0f && carTz == -9.0f && carUp == true) {
                //cond9
                carTx = carTx - 0.1f;
                //cout << "cond9" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx < -17.0f) {
                    carTx = -17.0f;
                }
            }

            else if (carTx > -22.0f && carTz < -4.0f && carTx <= -17.0f && carTz >= -9.0f && carUp == true) {
                //cond10
                carTx = carTx - 0.02f;
                carR = carR + 0.36f;
                carTz = carTz + 0.02f;
                //cout << "cond10" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx < -22.0f) {
                    carTx = -22.0f;
                }
                if (carTz > -4.0f) {
                    carTz = -4.0f;
                }
            }

            else if (carTx < -17.0f && carTz > 0.0f && carTx >= -22.0f && carTz <= 5.0f) {
                //cond12
                carTx = carTx + 0.05f;
                carR = carR - 0.9f;
                carTz = carTz - 0.05f;
                //cout << "cond12" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx > -17.0f) {
                    carTx = -17.0f;
                }
                if (carTz < 0.0f) {
                    carTz = 0.0f;
                    carTx = -17.0f;
                    carR = 0.0f;
                }
            }

            else if (carTx < 0.0f && carTz == 0.0f) {
                //cond13
                carTx = carTx + 0.1f;
                //cout << "cond13" << endl;
                //cout << carOn << " " << carTx << " " << carTz << endl;
                if (carTx > 0.0f) {
                    carTx = 0.0f;
                    carUp = true;
                    carOn = false;
                }
            }
        }

        //garage door
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.98f, 3.0f, -19.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.06f, garageS, 6.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        drawCube(VAO, lightingShader, model, color);
        //cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        if (!garageDoor) {
            if (garageS > -4.0f) {
                garageS -= 0.025f;
            }
            if (garageS <= -4.0f) {
                garageS = -4.0f;
            }
        }
        else {
            if (garageS < 0.0f) {
                garageS += 0.025;
            }
            if (garageS >= 0.0f) {
                garageS = 0.0f;
            }
        }

        //roof door
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, 8.98f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(roofS, 0.06, 5.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        drawCube(VAO, lightingShader, model, color);



        //bed 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 4.0f, 4.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 2.5f, -3.9f));
        color = glm::vec3(0.647f, 0.408f, 0.294f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.9f, 4.0f, 4.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.4f, 2.5f, -3.9f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 4.0f, 4.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.5f, -3.9f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 5.5f, 4.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 0.25f, -3.9f));
        color = glm::vec3(0.882f, 0.710f, 0.604f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //pillow
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 5.875f, 3.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, 2.0f, 0.25f));
        model = translateMatrix * rotateXMatrix * scaleMatrix;
        color = glm::vec3(0.9f, 0.9f, 0.9f);
        Cylinder_base c7 = Cylinder_base(0.5f, 36, 10, color, color, color, 32.0f);
        c7.drawSphere(lightingShader, model);

        //bed table 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 5.5f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.75f, 0.1f, -2.5f));
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 5.5f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.5f, -0.1f));
        color = glm::vec3(0.455f, 0.235f, 0.102f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.15f, 5.5f, -2.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.5f, 0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 5.5f, -2.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.5f, 0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.15f, 5.5f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.5f, -0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //bed 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 4.0f, -14.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 2.5f, 3.9f));
        color = glm::vec3(0.647f, 0.408f, 0.294f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.9f, 4.0f, -14.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.4f, 2.5f, 3.9f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 4.0f, -14.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.5f, 3.9f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 5.5f, -14.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 0.25f, 3.9f));
        color = glm::vec3(0.882f, 0.710f, 0.604f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //pillow 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 5.875f, -13.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, 2.0f, 0.25f));
        model = translateMatrix * rotateXMatrix * scaleMatrix;
        color = glm::vec3(0.9f, 0.9f, 0.9f);
        Cylinder_base c8 = Cylinder_base(0.5f, 36, 10, color, color, color, 32.0f);
        c8.drawSphere(lightingShader, model);

        //bed table 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 5.5f, -10.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.75f, 0.1f, 2.5f));
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 5.5f, -10.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.5f, 0.1f));
        color = glm::vec3(0.455f, 0.235f, 0.102f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.15f, 5.5f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.5f, -0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.9f, 5.5f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.5f, -0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.15f, 5.5f, -10.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, -1.5f, 0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);

        //2nd left Wall door L
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.98f, 4.0f, -6.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.06f, 4.0f, doorS));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        drawCube(VAO, lightingShader, model, color);
        //cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall door R
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.98f, 4.0f, -3.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.06f, 4.0f, -doorS));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        drawCube(VAO, lightingShader, model, color);
        //cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.1f, -1.0f + liftY, -3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, 0.5f, 3.8f));
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);
        //cube47.drawCubeWithTexture(lightingShaderWithTexture, model);

        if (liftUp) {
            if (liftDB == 1.5f) {
                liftY += 0.025f;
            }
            if (liftY >= 5.0f) {
                liftY = 5.0f;
                doorL2 = true;
            }
        }

        else {
            if (liftDU == 1.5f) {
                liftY -= 0.025f;
            }
            if (liftY <= 0.0f) {
                liftY = 0.0f;
                doorL1 = true;
            }
        }

        //lift door bottom L
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.5f, -1.0f, -3.98f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(liftDB, 5.0f, 0.06f));
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);
        //cube47.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift door bottom R
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.5f, -1.0f, -3.98f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-liftDB, 5.0f, 0.06f));
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);
        //cube47.drawCubeWithTexture(lightingShaderWithTexture, model);

        if (!doorL1) {
            liftDB += 0.025f;
            if (liftDB >= 1.5f) {
                liftDB = 1.5f;
            }
        }

        else {
            liftDB -= 0.025f;
            if (liftDB <= 0.0f) {
                liftDB = 0.0f;
            }
        }

        //lift door top L
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.5f, 4.0f, -3.98f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(liftDU, 5.0f, 0.06f));
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);
        //cube47.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift door top R
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.5f, 4.0f, -3.98f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-liftDU, 5.0f, 0.06f));
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        model = translateMatrix * scaleMatrix;
        drawCube(VAO, lightingShader, model, color);
        //cube47.drawCubeWithTexture(lightingShaderWithTexture, model);

        if (!doorL2) {
            liftDU += 0.025f;
            if (liftDU >= 1.5f) {
                liftDU = 1.5f;
            }
        }

        else {
            liftDU -= 0.025f;
            if (liftDU <= 0.0f) {
                liftDU = 0.0f;
            }
        }

        //bathroom tap
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.5f, 1.0f, -12.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.05f, -0.25f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //tap
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.05f, 0.25f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //food holder
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.9f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.455f, 0.235f, 0.102f);
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.9f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.5f, 0.5f, 0.2f);
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, -0.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.1f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.5f, 0.5f, 0.2f);
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 0.25f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.1f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.5f, 0.5f, 0.2f);
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.1f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.5f, 0.5f, 0.2f);
        drawCube(VAO, lightingShader, model, color);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.8f, 1.75f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.1f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.5f, 0.5f, 0.2f);
        drawCube(VAO, lightingShader, model, color);

        //kitchen box
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.9f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 0.1f, 10.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.6902f, 0.3412f, 0.3098f);
        drawCube(VAO, lightingShader, model, color);
        //cube15.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.9f, 3.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 0.1f, 10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube16.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.9f, 2.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, -0.1f, 10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube16.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.9f, 1.0f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-4.0f, 0.1f, -2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube17.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9f, -1.0f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 2.1f, -2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube17.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.9f, 3.5f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.0f, 0.1f, -1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube18.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.9f, 2.5f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.0f, -0.1f, -1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube18.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9f, 2.4f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.2f, -1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        //cube18.drawCubeWithTexture(lightingShaderWithTexture, model);
        
        //wheel
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -0.625f, 2.0f));
        AlignTranslateMatrix2 = glm::translate(identityMatrix, glm::vec3(0.0f, 0.075f, 0.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(wheelR), glm::vec3(0.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, 0.15f, 0.75f));
        model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * rotateXMatrix * rotateYMatrix * AlignTranslateMatrix2 * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        Cylinder_base c3 = Cylinder_base(0.5f, 36, 10, color, color, color, 32.0f);
        c3.drawSphere(lightingShader, model);

        //wheel
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, -0.625f, 2.0f));
        AlignTranslateMatrix2 = glm::translate(identityMatrix, glm::vec3(0.0f, 0.075f, 0.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(wheelR), glm::vec3(0.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, 0.15f, 0.75f));
        model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * rotateXMatrix * rotateYMatrix * AlignTranslateMatrix2 * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        Cylinder_base c4 = Cylinder_base(0.5f, 36, 10, color, color, color, 32.0f);
        c4.drawSphere(lightingShader, model);

        //wheel
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -0.625f, -2.0f));
        AlignTranslateMatrix2 = glm::translate(identityMatrix, glm::vec3(0.0f, -0.075f, 0.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(wheelR), glm::vec3(0.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, -0.15f, 0.75f));
        model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * rotateXMatrix * rotateYMatrix * AlignTranslateMatrix2 * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        Cylinder_base c5 = Cylinder_base(0.5f, 36, 10, color, color, color, 32.0f);
        c5.drawSphere(lightingShader, model);

        //wheel
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + carTx, 0.0f, -16.0f + carTz));
        AlignTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, -0.625f, -2.0f));
        AlignTranslateMatrix2 = glm::translate(identityMatrix, glm::vec3(0.0f, -0.075f, 0.0f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(wheelR), glm::vec3(0.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, -0.15f, 0.75f));
        model = translateMatrix * CarRotateYMatrix * AlignTranslateMatrix * rotateXMatrix * rotateYMatrix * AlignTranslateMatrix2 * scaleMatrix;
        color = glm::vec3(0.1f, 0.1f, 0.1f);
        Cylinder_base c6 = Cylinder_base(0.5f, 36, 10, color, color, color, 32.0f);
        c6.drawSphere(lightingShader, model);

        //toilet
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.5f, -1.15f, -18.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.375f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.95f, 0.95f, 0.95f);
        Cylinder c1 = Cylinder(0.5f, 36, 10, color, color, color, 32.0f);
        c1.drawSphere(lightingShader, model);

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
        lightingShaderWithTexture.setVec3("directionalLight.diffuse", 0.6f, 0.6f, 0.6f);
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

        //floor garage
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(14.0f, -0.1f, 8.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof garage bathroom
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-11.0f, 4.0f, -21.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(22.0f, -0.1f, 9.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube36.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd roof garage bathroom
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 4.1f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.0f, -0.1f, 8.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube37.drawCubeWithTexture(lightingShaderWithTexture, model);

        //floor bathroom
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

        //roof floor house 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-11.0f, 4.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(17.0f, -0.1f, 23.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube38.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd floor roof floor house 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 4.1f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(16.0f, -0.1f, 22.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube39.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof floor house 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 4.0f, -7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, -0.1f, 3.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube42.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd floor roof floor house 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 4.1f, -7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, -0.1f, 3.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube43.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof floor house 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, 4.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, -0.1f, 12.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube44.drawCubeWithTexture(lightingShaderWithTexture, model);

        /*
        //2nd floor roof floor house 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.9f, 4.1f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.1f, -0.1f, 12.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube45.drawCubeWithTexture(lightingShaderWithTexture, model);
        */

        //roof floor house kitchen
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 4.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, -0.1f, 11.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube40.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd floor roof floor kitchen
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 4.1f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, -0.1f, 10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube41.drawCubeWithTexture(lightingShaderWithTexture, model);

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

        //bathtub using bezier curve
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.5, -0.1, -15.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 2.0, 1.0));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translateMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", 32.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffMap8);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specMap8);
        glBindVertexArray(basinVAO);
        lightingShaderWithTexture.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesBasin.size(), GL_UNSIGNED_INT, (void*)0);

        //toilet using bezier curve
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.75, -0.3, -18.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75, 0.75, 0.75));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //rotateYMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translateMatrix * rotateZMatrix * scaleMatrix;
        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", 32.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffMap8);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specMap8);
        glBindVertexArray(basinVAO);
        lightingShaderWithTexture.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesBasin.size(), GL_UNSIGNED_INT, (void*)0);

        //bathroom basin using bezier curve
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.5, 1.0, -12.55));
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
        glBindVertexArray(basinVAO);
        lightingShaderWithTexture.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesBasin.size(), GL_UNSIGNED_INT, (void*)0);

        //pipe
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.5f, -1.15f, -12.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.56f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        Cylinder c2 = Cylinder(0.05f, 36, 10, color, color, color, 32.0f);
        c2.drawSphereTexture(lightingShaderWithTexture, model, diffMap7, specMap7);

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
        glBindVertexArray(basinVAO);
        lightingShaderWithTexture.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesBasin.size(), GL_UNSIGNED_INT, (void*)0);

        //pipe
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -1.15f, 0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.56f, 1.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        Cylinder c = Cylinder(0.05f, 36, 10, color, color, color, 32.0f);
        c.drawSphereTexture(lightingShaderWithTexture, model, diffMap7, specMap7);

        //roads
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, -30.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-10.0f, -0.1f, 50.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.0f, 0.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube13.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -1.0f, -30.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 50.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.0f, 0.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube13.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(30.0f, -0.1f, -10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.0f, 0.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube14.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(30.0f, -0.1f, 10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.0f, 0.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube14.drawCubeWithTexture(lightingShaderWithTexture, model);

        //kitchen box
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.9f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 2.0f, 10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube15.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.9f, 2.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 1.0f, 10.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube16.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.9f, -1.0f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-4.0f, 2.0f, -2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube17.drawCubeWithTexture(lightingShaderWithTexture, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.9f, 2.5f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.0f, 1.0f, -1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(1.0f, 1.0f, 1.0f);
        //drawCube(VAO, lightingShader, model, color);
        cube18.drawCubeWithTexture(lightingShaderWithTexture, model);

        //back wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.0f, 5.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube9.drawCubeWithTexture(lightingShaderWithTexture, model);

        //back wall out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, -1.0f, -20.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.2f, 7.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube19.drawCubeWithTexture(lightingShaderWithTexture, model);

        //front wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, 10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube9.drawCubeWithTexture(lightingShaderWithTexture, model);

        //front wall out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, -1.0f, 10.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.2f, 7.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube19.drawCubeWithTexture(lightingShaderWithTexture, model);

        //right wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 30.0f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube20.drawCubeWithTexture(lightingShaderWithTexture, model);

        //right wall out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.1f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 7.0f, 30.0f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube21.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube22.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 1 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 7.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 3.0f, -19.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 6.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube24.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 2 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, 3.0f, -19.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 6.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube25.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -13.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube22.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 3 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, -1.0f, -13.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 7.0f, 1.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube26.drawCubeWithTexture(lightingShaderWithTexture, model);
        
        //left Wall 4 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 7.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube27.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 3.0f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube28.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, 3.0f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube29.drawCubeWithTexture(lightingShaderWithTexture, model);
        
        //left Wall 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 14.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube30.drawCubeWithTexture(lightingShaderWithTexture, model);

        //left Wall 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.1f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 7.0f, 14.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube31.drawCubeWithTexture(lightingShaderWithTexture, model);

        //garage wall fb
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, -20.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 8.0f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube32.drawCubeWithTexture(lightingShaderWithTexture, model);

        //garage wall lr
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(14.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube30.drawCubeWithTexture(lightingShaderWithTexture, model);

        //bathroom wall lr 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, 3.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //bathroom wall lr 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube26.drawCubeWithTexture(lightingShaderWithTexture, model);

        //dinning wall fb 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 8.0f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube32.drawCubeWithTexture(lightingShaderWithTexture, model);

        //dinning wall fb 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 3.0f, 8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.0f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //dinning wall lr 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube34.drawCubeWithTexture(lightingShaderWithTexture, model);

        //dinning wall lr 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 3.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //kitchen wall lr 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 3.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //kitchen wall lr 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(6.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube35.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift wall fb 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 10.0f, 4.0f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube47.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift wall lr 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 10.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube46.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift wall lr 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.5f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 10.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube46.drawCubeWithTexture(lightingShaderWithTexture, model);

        //lift wall lr 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 4.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube48.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 5.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube34.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 1 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.1f, 4.0f, -15.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.1f, 5.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube50.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 8.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 8.0f, -15.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube49.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 5.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube22.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 3 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 4.0f, -15.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 5.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 4.0f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube34.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall 1 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.1f, 4.0f, 5.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.1f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube50.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 8.0f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall 2 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 8.0f, 5.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 1.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube49.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 4.0f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube22.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall 3 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 4.0f, 5.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube23.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 8.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube51.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 1 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.1f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 8.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube53.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 8.0f, -6.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 3.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube52.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 2 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.1f, 8.0f, -6.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 3.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube54.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 4.0f, -3.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 8.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube51.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 3 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.1f, 4.0f, -3.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 8.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube53.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 1 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 6.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube35.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 2 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 8.0f, -9.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 3 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 8.0f, -3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube33.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 4 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 4.0f, -1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 5.0f, 6.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube35.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 1 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 4.0f, -7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(7.0f, 5.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube55.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd back wall 2 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 4.0f, -3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(7.0f, 5.0f, 0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube55.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd right Wall 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 20.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube9.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd right Wall 1 out
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.1f, 6.0f, -15.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 3.0f, 20.2f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube56.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 9.0f, -16.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(11.0f, 0.1f, 22.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube57.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 9.0f, -7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 0.1f, 13.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube42.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 9.0f, -16.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 0.1f, 4.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube58.drawCubeWithTexture(lightingShaderWithTexture, model);

        //roof 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, 9.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 5.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        //drawCube(VAO, lightingShader, model, color);
        cube59.drawCubeWithTexture(lightingShaderWithTexture, model);

        float stair_y = 4.0f, stair_z = -12.0f;

        for (int i = 0; i < 10; i++) {
            //floor stair
            translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, stair_y, stair_z));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -0.1f, 0.5f));
            model = translateMatrix * scaleMatrix;
            //color = glm::vec3(0.494f, 0.514f, 0.541f);
            //drawCube(VAO, lightingShader, model, color);
            cube10.drawCubeWithTexture(lightingShaderWithTexture, model);

            stair_z = stair_z + 0.5f;

            //wall stair
            translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, stair_y, stair_z + 0.1f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -0.5f, -0.1f));
            model = translateMatrix * scaleMatrix;
            //color = glm::vec3(0.659f, 0.820f, 0.843f);
            //drawCube(VAO, lightingShader, model, color);
            cube10.drawCubeWithTexture(lightingShaderWithTexture, model);

            stair_y = stair_y - 0.5f;
        }

        float stair_y2 = 9.0f, stair_z2 = -7.0f;

        for (int i = 0; i < 10; i++) {
            //floor stair
            translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, stair_y2, stair_z2 - 0.4));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -0.1f, 0.5f));
            model = translateMatrix * scaleMatrix;
            //color = glm::vec3(0.494f, 0.514f, 0.541f);
            //drawCube(VAO, lightingShader, model, color);
            cube10.drawCubeWithTexture(lightingShaderWithTexture, model);

            stair_z2 = stair_z2 - 0.5f;

            //wall stair
            translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, stair_y2, stair_z2 + 0.1f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -0.5f, -0.1f));
            model = translateMatrix * scaleMatrix;
            //color = glm::vec3(0.659f, 0.820f, 0.843f);
            //drawCube(VAO, lightingShader, model, color);
            cube10.drawCubeWithTexture(lightingShaderWithTexture, model);

            stair_y2 = stair_y2 - 0.5f;
        }

        //2nd back wall door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 4.0f, -15.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 4.0f, 0.1f));
        //color = glm::vec3(0.0f, 0.0f, 0.0f);
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd front wall door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 4.0f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 4.0f, -0.1f));
        //color = glm::vec3(0.494f, 0.514f, 0.541f);
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 2 2 door 1
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 4.0f, -9.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, 2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //2nd left Wall 3 2 door 2
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 4.0f, -1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, -2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);
        
        //out door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, 2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.624f, 0.416f, 0.310f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //out door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, -2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.624f, 0.416f, 0.310f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //bathroom door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 4.0f, -0.1f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.624f, 0.416f, 0.310f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //dinning to kitchen door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -1.0f, 8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.0f, 2.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.624f, 0.416f, 0.310f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //dinning door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 4.0f, -0.1f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.624f, 0.416f, 0.310f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //kitchen door
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, -1.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 4.0f, -0.1f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        //color = glm::vec3(0.624f, 0.416f, 0.310f);
        //drawCube(VAO, lightingShader, model, color);
        cube11.drawCubeWithTexture(lightingShaderWithTexture, model);

        //TV
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 1.0f, -12.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 2.0f, 0.1f));
        //color = glm::vec3(0.9f, 0.9f, 0.9f);
        model = translateMatrix * scaleMatrix;
        //drawCube(VAO, lightingShader, model, color);
        cube12.drawCubeWithTexture(lightingShaderWithTexture, model);

        //trianlge.drawTriangleWithTexture(lightingShaderWithTexture, identityMatrix);
        
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

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
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
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            birdEyePosition.z += birdEyeSpeed * deltaTime; // Move backward along Z
            birdEyeTarget.z += birdEyeSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            birdEyePosition.x -= birdEyeSpeed * deltaTime; // Move forward along Z
            birdEyeTarget.x -= birdEyeSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            birdEyePosition.x += birdEyeSpeed * deltaTime; // Move backward along Z
            birdEyeTarget.x += birdEyeSpeed * deltaTime;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        directionLightOn = !directionLightOn;
    }

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

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (garageDoor) {
            carOn = true;
        }  
    }

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        garageDoor = true;
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        if (!carOn) {
            garageDoor = false;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (liftY == 5.0f) {
            liftUp = false;
            doorL2 = false;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (liftY == 0.0f) {
            liftUp = true;
            doorL1 = false;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (varSwingAngle < 45.0f) {
            varSwingAngle += 15.0f;
            if (varSwingAngle > 45.0f) {
                varSwingAngle = 45.0f;
            }
        }
        if (swingAngle >= 0.0f) {
            swingAngle += 15.0f;
            if (swingAngle > 45.0f) {
                swingAngle = 45.0f;
            }
        }
        else {
            swingAngle -= 15.0f;
            if (swingAngle < -45.0f) {
                swingAngle = -45.0f;
            }
        }
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
            float v = t;     // Along the curve
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