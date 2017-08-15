//
//  planeModel.h
//  Projection
//
//  Created by Dingyue on 7/19/17.
//  Copyright © 2017 Dingyue. All rights reserved.
//

#ifndef planeModel_h
#define planeModel_h

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int loadTexture(string const &path);

class PlaneModel{
public:
    /* Model Data */
    unsigned int planeVAO, planeVBO;
    unsigned int floorTexture;
    
    /* Functions */
    PlaneModel(string const &path){
        loadModel(path);
    }
    
    void Draw(){
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    
private:
    /* Functions */
    void loadModel(string const &path){
        float planeVertices[] = {
            // positions          // texture Coords
            7.0f, -5.0f,  -12.0f,  0.0f, 1.0f,
            7.0f, 5.0f, -12.0f,  0.0f, 0.0f,
            -7.0f, 5.0f, -12.0f,  1.0f, 0.0f,
            
            -7.0f, 5.0f,  -12.0f,  1.0f, 0.0f,
            -7.0f, -5.0f, -12.0f,  1.0f, 1.0f,
            7.0f, -5.0f, -12.0f,  0.0f, 1.0f
        };
        
        // plane VAO
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        
        // load textures
        // -------------
        floorTexture = loadTexture(path);
    }
    
    unsigned int loadTexture(string const &path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        
        int width, height, nrComponents;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
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
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
        
        return textureID;
    }
};

#endif /* planeModel_h */
