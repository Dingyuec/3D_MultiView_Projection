#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <vector>

#define PI 3.14159265

// Default camera values
const float YAW        = -90.0f;
const float PITCH      =  0.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 ViewPoint;
    glm::vec3 WorldUp;
    
    // Eular Angles
    float Yaw;
    float Pitch;
    float Zoom;
    
    // Rotation Axis
    char Axis;
    
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3 viewPoint = glm::vec3(0.0f, 0.0f, 0.0f), char axis = 'y', float zoom = 45.0f, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
    {
        Position = position;
        ViewPoint = viewPoint;
        Axis = axis;
        Zoom = zoom;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
    }
    
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float viewX, float viewY, float viewZ, char axis, float zoom, float upX, float upY, float upZ, float yaw, float pitch)
    {
        Position = glm::vec3(posX, posY, posZ);
        ViewPoint = glm::vec3(viewX, viewY, viewZ);
        Axis = axis;
        Zoom = zoom;
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
    }
    
    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix(float cur_deg)
    {
        glm::mat4 view;
        float cur_rad = cur_deg * PI / 180.0;
        glm::vec3 tempPosition = GetCameraPos(Position, Axis, cur_rad);
        view = glm::lookAt(tempPosition, ViewPoint, WorldUp);
        return view;        
    }
    
    glm::vec3 GetCameraPos(glm::vec3 pos, char axis, float cur_rad)
    {
        GLfloat radius;
        GLfloat cam_1 = sin(cur_rad);
        GLfloat cam_2 = cos(cur_rad);
        
        switch (axis) {
            case 'x':
                radius = sqrtf(powf(pos.y, 2.0) + powf(pos.z, 2.0));
                return glm::vec3(pos.x, cam_1 * radius, cam_2 * radius);
                break;
            case 'y':
                radius = sqrtf(powf(pos.x, 2.0) + powf(pos.z, 2.0));
                return glm::vec3(cam_1 * radius, pos.y, cam_2 * radius);
                break;
            case 'z':
                radius = sqrtf(powf(pos.x, 2.0) + powf(pos.y, 2.0));
                return glm::vec3(cam_1 * radius, cam_2 * radius, pos.z);
                break;
            default:
                break;
        }
        
        return pos;
    }
    
};
#endif
