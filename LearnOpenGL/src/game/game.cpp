#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <random>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
bool shootTarget(glm::vec3 vectorOfBullet, glm::vec3 targetViewVector);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


// settings
const unsigned int SCR_WIDTH = 1040;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 camDirection = camera.Front + camera.Position;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// variables for the gun
const float GUN_DISTANCE = 1.5f;
const float GUN_HEIGHT = 1.5f;
glm::vec3 GunPosition = glm::vec3(0.0f, GUN_HEIGHT, GUN_DISTANCE);


float Yaw;
float Pitch;

// INITIALIZE TERRORIST POSITIONS

float level1Height = 25.0f;
float level2Height = 49.7f;
float level3Height = 71.0f;

float level1Distance = -35.0f;
float level2Distance = -51.5f;
float level3Distance = -61.5f;

// SETUP DIFF POSITIONS OF SOLDIERS
float widthOfCastle = 30.0f;

std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> distr(int(-widthOfCastle), int(widthOfCastle));

float level1SoldierPosition = float(distr(gen));
float level2SoldierPosition = float(distr(gen));
float level3SoldierPosition = float(distr(gen));

float halfHeightOfTerrorist = 8.0f;
float halfWidthOfTerrorist = 2.0f;

int score = 0;
int ammo = 10;

glm::vec3 terroristPositions[3] = {
                glm::vec3(level1SoldierPosition, level1Height , level1Distance),
                glm::vec3(level2SoldierPosition, level2Height, level2Distance),
                glm::vec3(level3SoldierPosition, level3Height, level3Distance),
};

// TEST
bool shoot = false;
glm::vec3 modelSkyBoxPos = glm::vec3(1.0f);
//glm::mat4 modelSkyBox1 = glm::mat4(1.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PRINCE K", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader gunShader("model.vs", "model.fs");
    Shader castleShader("model.vs", "model.fs");
    Shader terroristShader("model.vs", "model.fs");
    Shader aimShader("model.vs", "model.fs");
    Shader skyBoxShader("model.vs", "model.fs");
    //TEST
    //Shader skyBoxShader1("model.vs", "model.fs");

    // load models
    // -----------

    Model gunModel(FileSystem::getPath("/resources/objects/ว่-47/ว่-47.obj"));
    Model castleModel(FileSystem::getPath("/resources/objects/Castle/Castle OBJ.obj"));
    Model terroristModel(FileSystem::getPath("/resources/objects/nanosuit/nanosuit.obj"));
    Model aimModel(FileSystem::getPath("/resources/objects/aim/aim.obj"));
    Model skyBoxModel(FileSystem::getPath("/resources/objects/skybox/skybox1.obj"));
    // TEST
    //Model skyBoxModel1(FileSystem::getPath("/resources/objects/skybox/skybox1.obj"));
    shoot = false;

    printf("----------------------------------------\n");
    printf("------------- GAME START ---------------\n");
    printf("----------------------------------------\n");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, true);

        glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gunShader.use();
        castleShader.use();
        terroristShader.use();
        aimShader.use();
        skyBoxShader.use();
        // TEST
        //skyBoxShader1.use();

        //----------------------------
        // GUN SETUP
        // 
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        gunShader.setMat4("projection", projection);
        gunShader.setMat4("view", view);

        // render GUN
        glm::mat4 modelGun = glm::mat4(1.0f);
        modelGun = glm::translate(modelGun, GunPosition); // translate it down so it's at the center of the scene
        modelGun = glm::scale(modelGun, glm::vec3(0.008f, 0.008f, 0.008f));	// it's a bit too big for our scene, so scale it down

        Yaw = camera.Yaw;
        Pitch = camera.Pitch;
        float anglePitch = fmod(Pitch, 360.0f);

        // ROTATION BY X AXIS 
        modelGun = glm::rotate(modelGun, glm::radians(anglePitch), glm::vec3(0.5f, 0.0f, 0.0f));

        // ROTATION BY Y AXIS 
        float angleYaw = fmod(Yaw, 360.0f);

        modelGun = glm::rotate(modelGun, -glm::radians(angleYaw - 95.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        camDirection = camera.Front + camera.Position;
        float xcoord = camDirection.x;
        float ycoord = camDirection.y;
        float zcoord = camDirection.z;

        glm::vec3 MouseGunPosition = glm::vec3(xcoord, ycoord - (ycoord - GUN_HEIGHT), zcoord);
        GunPosition = MouseGunPosition;

        gunShader.setMat4("model", modelGun);
        gunModel.Draw(gunShader);

        //----------------------------
        // AIM SETUP
        // 
        // 
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        aimShader.setMat4("view", view);
        aimShader.setMat4("projection", projection);

        // render AIM
        glm::mat4 modelAim = glm::mat4(1.0f);
        modelAim = glm::translate(modelAim, glm::vec3(xcoord, ycoord, zcoord));
        modelAim = glm::scale(modelAim, glm::vec3(0.02f, 0.02f, 0.02f));

        modelAim = glm::rotate(modelAim, -glm::radians(angleYaw - 95.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        aimShader.setMat4("model", modelAim);
        aimModel.Draw(aimShader);

        //------------------------------------------------------------------
        // ----------------------------------------------------------------
        // INVISIBLE VECTOR BULLET
        // 
        if (shoot == false) {
            modelSkyBoxPos = glm::vec3(xcoord, ycoord, zcoord);
        }



        //------------------------------
        // CASTLE SETUP
        // 
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        castleShader.setMat4("projection", projection);
        castleShader.setMat4("view", view);

        // render CASTLE
        glm::mat4 modelCastle = glm::mat4(1.0f);
        modelCastle = glm::translate(modelCastle, glm::vec3(0.0f, -1.0f, 0.0f));
        castleShader.setMat4("model", modelCastle);
        castleModel.Draw(castleShader);

        //------------------------------
        // SKYBOX SETUP
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        skyBoxShader.setMat4("projection", projection);
        skyBoxShader.setMat4("view", view);

        // render SKYBOX
        glm::mat4 modelSkyBox = glm::mat4(1.0f);
        modelSkyBox = glm::translate(modelSkyBox, glm::vec3(0.0f, -1.0f, 0.0f));
        modelSkyBox = glm::scale(modelSkyBox, glm::vec3(20.0f, 20.0f, 20.0f));
        skyBoxShader.setMat4("model", modelSkyBox);
        skyBoxModel.Draw(skyBoxShader);



        //------------------------------
        // TERRORIST SETUP
        // 
        glm::mat4 modelTerrorist[3] = {};
        for (unsigned int i = 0; i < 3; i++) {
            modelTerrorist[i] = glm::mat4(1.0f);
            modelTerrorist[i] = glm::scale(modelTerrorist[i], glm::vec3(0.3f, 0.3f, 0.3f));
            modelTerrorist[i] = glm::translate(modelTerrorist[i], terroristPositions[i]);
            terroristShader.setMat4("model", modelTerrorist[i]);
            terroristModel.Draw(terroristShader);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        GunPosition += camera.Front * camera.MovementSpeed * deltaTime;
        if (GunPosition.y != GUN_HEIGHT) {
            GunPosition.y = GUN_HEIGHT;
        }
        //printf("%f %f %f\n", GunPosition.x, GunPosition.y, GunPosition.z);
    }


    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        GunPosition -= camera.Front * camera.MovementSpeed * deltaTime;
        if (GunPosition.y != GUN_HEIGHT) {
            GunPosition.y = GUN_HEIGHT;
        }
    }


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        // ROLL + move the model in line with the camera vector
        GunPosition -= camera.Right * camera.MovementSpeed * deltaTime;
        if (GunPosition.y != GUN_HEIGHT) {
            GunPosition.y = GUN_HEIGHT;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        // same as left
        GunPosition += camera.Right * camera.MovementSpeed * deltaTime;
        if (GunPosition.y != GUN_HEIGHT) {
            GunPosition.y = GUN_HEIGHT;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------

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
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {\
        glm::vec3 camera_front = camera.Front;
        bool targetShot = shootTarget(modelSkyBoxPos, camera_front);
        if (targetShot && ammo > 0) {
            printf("----------------------------------------\n");
            printf("------------ TARGET SHOT ---------------\n");
            printf("-------- score : %d -- ammo: %d --------\n", score, ammo);
            printf("----------------------------------------\n");
        }
        else if(ammo > 0){
            printf("----------------------------------------\n");
            printf("----------- TARGET MISSED --------------\n");
            printf("--------- score : %d -- ammo: %d ---------\n", score, ammo);
            printf("----------------------------------------\n");   
        }
        else {
            if (ammo == 0) {
                printf("----------------------------------------\n");
                printf("----------------------------------------\n");
                printf("-------------- GAME OVER ---------------\n");
                printf("------------- out of ammo --------------\n");
                printf("----------------------------------------\n");
                printf("--------------- SCORE: -----------------\n");
                printf("----------------- %d/10 ------------------\n", score);
                printf("----------------------------------------\n");
            }
        }
        
    }
        
}

bool shootTarget(glm::vec3 position, glm::vec3 direction) {
    float xcoord = direction.x;
    float ycoord = direction.y;
    float zcoord = direction.z;

    float speed = 1.0f;
    double elapsed_time = 0.0f;


    while (position.x < 100.0f && position.x > -100.0f &&
        position.y < 100.0f && position.y > -100.0f &&
        position.z < 100.0f && position.z > -100.0f) {

        elapsed_time = glfwGetTime() / 1000.0f;

        position.x += (float)(direction.x * speed * elapsed_time);
        position.y += (float)(direction.y * speed * elapsed_time);
        position.z += (float)(direction.z * speed * elapsed_time);

        for (int i = 0; i < 3; i++) {
            if (position.x < terroristPositions[i].x + halfHeightOfTerrorist && position.x > terroristPositions[i].x - halfHeightOfTerrorist
                && position.y < terroristPositions[i].y + halfWidthOfTerrorist && position.y > terroristPositions[i].y - halfWidthOfTerrorist
                && position.z < terroristPositions[i].z + 1.0f && position.z > terroristPositions[i].z - 1.0f) {
                
                if (ammo != 0) {
                    // randomize the position of the terrorist
                    float previotusPosition = terroristPositions[i].x;
                    while (terroristPositions[i].x == previotusPosition) {
                        terroristPositions[i].x = float(distr(gen));
                    }
                }
                
                score += 1;
                ammo -= 1;

                shoot = false;
                return true;
            }
        }
         //printf("-------BULLET POSITIONS-----------\n %f %f %f\n", position.x, position.y, position.z);
        }

       ammo -= 1;
       shoot = false;
       return false;
       //printf("-------COORDINATES-----------\n %f %f %f\n", xcoord, ycoord, zcoord);
    }
