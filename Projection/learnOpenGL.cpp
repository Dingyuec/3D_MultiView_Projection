#include <glad/glad.h> // before glfw
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <string>
#include <math.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#define png_infopp_NULL (png_infopp)NULL

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int Snapshot(char *file_name, unsigned int x, unsigned int y, unsigned long width, unsigned long height);

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1000;

// camera
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT/2;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// snapshot
int file_index = 0;
char *res_path = "/Users/cui/Kneron/ModelProjection/Projection/Results/";

// Insert camera/view point position (x, y, z)/axis/degree range
float camera_x, camera_y, camera_z;
float view_x, view_y, view_z;
char axis;
float start_deg, end_deg;
int num_out;
float fov;
char texture_name[20];

int main()
{
//    cout << "Please insert 3 float number (x, y, z) for camera position." <<endl;
//    cin >> camera_x >> camera_y >> camera_z;
//    cout << "Please insert 3 float number (x, y, z) for view point position." << endl;
//    cin >> view_x >> view_y >> view_z;
//    cout << "Please insert an axis (x / y / z)." << endl;
//    cin >> axis;
//    cout << "Please insert rotation range (a, b) in degree." << endl;
//    cin >> start_deg >> end_deg;
//    cout << "Please insert the number of pictures output." <<endl;
//    cin >> num_out;
//    cout << "Please insert field of view (0, 180)." <<endl;
//    cin >> fov;
    cout << "Please insert the name of texture file (eg. Plastic.jpg)." <<endl;
    cin >> texture_name;
    
    // camera:
//    Camera camera(glm::vec3(camera_x, camera_y, camera_z), glm::vec3(view_x, view_y, view_z), axis, fov);
    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 'y', 45.0f);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // build and compile shaders
    // -------------------------
    Shader ourShader("/Users/cui/openGLfiles/shader.vs", "/Users/cui/openGLfiles/shader.fs");
    
    // load models
    // -----------
    Model ourModel("/Users/cui/Downloads/sofa/georgetti.obj", texture_name);
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    GLfloat scaleFactor = float (1 / pow(ourModel.volume.x * ourModel.volume.y * ourModel.volume.z, 1.0/3.0));

    
    // render loop
    // -----------
    
    float current_deg = start_deg;
    float rotation_step = (end_deg - start_deg) / (num_out - 1);
    
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix(current_deg);
        current_deg += rotation_step;
        
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        
        // render the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(ourModel.pivot.x * -scaleFactor,
                                                ourModel.pivot.y * -scaleFactor,
                                                ourModel.pivot.z * -scaleFactor));
        
        model = glm::scale(model, glm::vec3(scaleFactor , scaleFactor , scaleFactor));

        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
        
        // make window snapshot
        char file_path[100] = "/Users/cui/Kneron/ModelProjection/Projection/Results/";
        char index[10];
        sprintf(index, "%d", file_index);
        strcat(file_path, index);
        char suffix[10] = ".png";
        strcat(file_path, suffix);
        Snapshot(file_path, 0, 0, SCR_WIDTH * 2, (SCR_HEIGHT - 100) * 2);
        ++ file_index;
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        if (current_deg > end_deg){
            glfwSetWindowShouldClose(window, true);
        }
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


int Snapshot(char *file_name, unsigned int x, unsigned int y, unsigned long width, unsigned long height)
{
    FILE *fp;
    unsigned long i;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;
    png_byte *image;
    png_bytep *row_pointers;
    
    fp = fopen(file_name, "wb");
    if (fp == NULL)
        return -1;
    
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (png_ptr == NULL)
    {
        fclose(fp);
        return -1;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, png_infopp_NULL);
        return -1;
    }
    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }
    
    png_init_io(png_ptr, fp);
    
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof (png_color));
    png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
    
    png_write_info(png_ptr, info_ptr);
    
    png_set_packing(png_ptr);
    
    image = (png_byte *)malloc(width * height * 3 * sizeof(png_byte));
    if(image == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }
    
    row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));
    if(row_pointers == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        free(image);
        image = NULL;
        return -1;
    }
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)image);
    
    for (i = 0; i < height; i++)
    {
        row_pointers[i] = (png_bytep)image + (height - i) * width * 3;
    }
    
    png_write_image(png_ptr, row_pointers);
    
    png_write_end(png_ptr, info_ptr);
    
    png_free(png_ptr, palette);
    palette = NULL;
    
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    free(row_pointers);
    row_pointers = NULL;
    
    free(image);
    image = NULL;
    
    fclose(fp);
    
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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


