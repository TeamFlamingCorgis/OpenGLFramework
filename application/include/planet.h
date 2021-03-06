//
// Created by Rosemary Adejoh && Liselot Ramirez on 26.10.18.
//
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

//ASSIGNMENT 1 - RENDER PLANETS
struct Planet {
    string name; //name
    float size; //size
    float distance; //distance
    float rotation; //rotation
    float orbitTilt;//orbit tilt
    string type;//sun, planet or star
    glm::vec3 color; //literally the color we need for each planet!
};


//OPENGL_FRAMEWORK_APPLICATION_SYSTEM_H
