#include <glad/glad.h> // before glfw
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "planeModel.h"

#include <iostream>
#include <string>
#include <math.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#define png_infopp_NULL (png_infopp)NULL

//---------------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int Snapshot(char *file_name, unsigned int x, unsigned int y, unsigned long width, unsigned long height);

// settings
const unsigned int WIDTH = 2000;
const unsigned int HEIGHT = 1500;
const unsigned long SCR_WIDTH = 3350;
const unsigned long SCR_HEIGHT = 1850;

// snapshot
int file_index = 0;
char *res_path = "/Users/cui/Kneron/ModelProjection/Projection/Results/";

// coodinates
glm::vec2 left_bottom;
glm::vec2 width_height;

// Insert camera/view point position (x, y, z)/axis/degree range
float camera_x, camera_y, camera_z;
float view_x, view_y, view_z;
float start_deg, end_deg;
float fov;
char contour_y_n;
char axis;
int num_out;

//---------------------------------------------------------------------------------

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
//    cout << "Do you want to output contour image? (y/n)" << endl;
//    cin >> contour_y_n;
    camera_x = 0.0;
    camera_y = 0.0;
    camera_z = 9.0;
    view_x = 0.0;
    view_y = 0.0;
    view_z = 0.0;
    axis = 'y';
    start_deg = 0;
    end_deg = 90;
    num_out = 10000;
    fov = 45;
    contour_y_n = 'n';
    GLfloat scale = 1.0;
    
    // rotation angle:
    float current_deg = start_deg;
    float rotation_step = (end_deg - start_deg) / (num_out - 1);
    
    // if output contour image
    bool if_contour;
    if (contour_y_n == 'y') if_contour = true;
    else    if_contour = false;
    
    // camera:
    Camera camera(glm::vec3(camera_x, camera_y, camera_z), glm::vec3(view_x, view_y, view_z), axis, fov);
    
    //------------------------------------------------------------------------------
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Only for mac
    
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Output", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    
    //------------------------------------------------------------------------------
    // build and compile shaders and models for loaded model
    Shader ourShader("/Users/cui/openGLfiles/shader.vs",
                     "/Users/cui/openGLfiles/shader.fs");
    
    Model ourModel("/Users/cui/Desktop/models/1.obj",
                   "/Users/cui/Desktop/textures/2.jpg");
    
    GLfloat scaleFactor = float (1 / pow(ourModel.volume.x * ourModel.volume.y * ourModel.volume.z, 1.0/3.0));
    
    
    // build and compile shaders and models for plane
    Shader planeShader("/Users/cui/openGLfiles/planeShader.vs",
                       "/Users/cui/openGLfiles/planeShader.fs");
    
    PlaneModel planeModel("/Users/cui/Desktop/backgrounds/1.jpg");
    
    
    // build and compile shaders for contour
    Shader shaderSingleColor("/Users/cui/openGLfiles/singleColorShader.vs",
                             "/Users/cui/openGLfiles/singleColorShader.fs");
    
    //------------------------------------------------------------------------------
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        // view/projection transformations
        glm::mat4 model;
        glm::mat4 plane_matrix;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)WIDTH / (float)HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix(current_deg);
        
        // enable shader
        if (if_contour){
            shaderSingleColor.use();
            shaderSingleColor.setMat4("view", view);
            shaderSingleColor.setMat4("projection", projection);
        }
        else{
            planeShader.use();
            planeShader.setMat4("projection", projection);
            planeShader.setMat4("model", plane_matrix);
            planeModel.Draw();
            
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
        }
        
        model = glm::translate(model, glm::vec3(ourModel.pivot.x * -scaleFactor,
                                                ourModel.pivot.y * -scaleFactor,
                                                ourModel.pivot.z * -scaleFactor));
        model = glm::scale(model, glm::vec3(scaleFactor , scaleFactor , scaleFactor));
        
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
        
        current_deg += rotation_step;
        
        //------------------------------------------------------------------------------
        // make window snapshot
        char file_path[100] = "/Users/cui/Kneron/ModelProjection/Projection/Results/";
        char index[10];
        sprintf(index, "%d", file_index);
        strcat(file_path, index);
        char suffix[10] = ".png";
        strcat(file_path, suffix);
        
        Snapshot(file_path, 0, 0, SCR_WIDTH, SCR_HEIGHT);
        ++ file_index;
        
        //------------------------------------------------------------------------------
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        if (current_deg > end_deg){
            glfwSetWindowShouldClose(window, true);
        }
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// snapshot the screen and save it in file path
// -------------------------------------------
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
        row_pointers[i] = (png_bytep)image + (height - i - 1) * width * 3;
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






