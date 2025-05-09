#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "shaderLoader.h"
#include "renderObjects.hpp"


typedef struct Vertex{
    glm::vec2 vertex;
    glm::vec3 color;
}Vertex;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

void drawLine(int pointSize, double lineWidth, double *color, double *start, double *end);

// settings
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int MAP_WIDTH  = 24;
const unsigned int MAP_HEIGHT = 24;

int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


int main(){

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    double posX    = 12, posY = 8;  //x and y start position
    double dirX    = -1, dirY = 0; //initial direction vector
    double planeX  = 0, planeY = 0.66; //the 2d raycaster version of camera plane
    double time    = 0; // current frame
    double oldTime = 0; // last frame


    ShaderLoader shaders("./shaders/vShader.vs", "./shaders/fShader.fs");
    

    unsigned int program = shaders.ID;
    glUseProgram(program);

    std::vector<float> color(3);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        for(int x = 0; x < SCR_WIDTH; x++)
        {
            //calculate ray position and direction
            double cameraX = 2 * x / (double)SCR_WIDTH - 1; //x-coordinate in camera space
            double rayDirX = dirX + planeX * cameraX; 
            double rayDirY = dirY + planeY * cameraX; 

            // Square coordinate of the map
            int mapX = int(posX);
            int mapY = int(posY);

            // lenght of ray from current position to next x or y-side
            double sideDistX;
            double sideDistY;

            //length of ray from one x or y-side to next x or y-side
            double deltaDistX = std::abs(1/rayDirX);
            double deltaDistY = std::abs(1/rayDirY);

            double perpWallDist;

            // indicates the direction to go for in x or y (either +1 or -1)
            int stepX;
            int stepY;

            int hit = 0;
            int side;

            if(rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }
            if(rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            while (hit == 0){
                if(sideDistX < sideDistY){
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }else{
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                // Check if ray has hit a wall
                if(worldMap[mapX][mapY] > 0) hit = 1;    
            }


            if (side == 0) {
                perpWallDist = (sideDistX - deltaDistX);
            }else{
                perpWallDist = (sideDistY - deltaDistY);
            }

            int lineHeight = (int)(SCR_HEIGHT / perpWallDist);

            int drawStart = -lineHeight / 2 + SCR_HEIGHT / 2;
            if(drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + SCR_HEIGHT / 2;
            if(drawEnd >= SCR_HEIGHT) drawEnd = SCR_HEIGHT - 1;           

            switch(worldMap[mapX][mapY])
            {
                case 1:  color = {1.0f, 0.0f, 0.0f}; break; //red
                case 2:  color = {0.0f, 1.0f, 0.0f}; break; //green
                case 3:  color = {0.0f, 0.0f, 1.0f}; break; //blue
                case 4:  color = {0.65f, 0.3f, 1.0f}; break; 
                default: color = {1.0f, 0.0f, 1.0f}; break; 
            }

            if (side == 1){
                color[0] = color[0]/2;
                color[1] = color[1]/2;
                color[2] = color[2]/2;
            }
            

            //Transforming to NDC coordinates
            float xDrawNDC = (float) x / SCR_WIDTH * 2 - 1;
            float drawStartNDC = (float)drawStart / SCR_HEIGHT * 2 - 1;
            float drawEndNDC = (float)drawEnd / SCR_HEIGHT * 2 - 1;

            g_RenderLine(xDrawNDC, drawStartNDC, xDrawNDC, drawEndNDC, color);

            color.clear();   
        }


        oldTime = time;
        time = glfwGetTime();
        double frameTime = (time - oldTime); //frameTime is the time this frame has taken, in seconds
        // std::cout << (1.0 / frameTime) << std::endl;

        double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
        double rotSpeed = frameTime * 3.0; //the constant value is in radians/second

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
            if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
        }
    
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
          if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
          if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }
    
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            //both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
          //both camera direction and camera plane must be rotated
          double oldDirX = dirX;
          dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
          dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
          double oldPlaneX = planeX;
          planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
          planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
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
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}




