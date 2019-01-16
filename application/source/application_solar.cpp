#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

//Define number of celestial bodies
#define NUMBER_OF_CEL_BODIES 10
//Define the orbit and stars
#define  NUMBER_OF_STARS 1000
#define  ORBIT_THICKNESS 50

//use constants from glm
#ifndef GLM_GTC_constants
#define GLM_GTC_constants
#endif
#include <glm/gtc/constants.hpp>


//Initialize the models here, because otherwise it won't compile
model planet_model{};
model star_model{};
model orbit_model{};
model skybox_model{};
//ASSIGNMENT 5
model screenquad_model{};

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 , planet_object{}, star_object{}, orbit_object{}, skybox_object{}, screenquad_object{}
{

    //ASSIGNMENT 2 - BUFFER FOR STARS
    //Remember: THE ORDER MATTERS! Otherwise the rendering is shitty
        star_object.num_elements = NUMBER_OF_STARS;
        for (int i = 0; i < star_object.num_elements; i++){

            starBuffer.push_back(randPos());
            starBuffer.push_back(randPos());
            starBuffer.push_back(randPos());

            starBuffer.push_back(randCol());
            starBuffer.push_back(randCol());
            starBuffer.push_back(randCol());
        }

    //ASSIGNMENT 2 - BUFFER FOR ORBIT
    //How thick do you want the orbit?
    orbit_object.num_elements = ORBIT_THICKNESS;
    makeOrbits();

    //ASSIGNMENT 4 - Load textures
    //load textures
    loadAllTextures();
    //load normal map
    loadNormalMap(GL_TEXTURE12);

    //ASSIGNMENT 5 - Initialize frame buffer
    setupOffscreenRendering();

    //MODELS GO HERE
    //planet
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD | model::TANGENT);
    //stars only use colors
    star_model = {starBuffer, model::POSITION | model::NORMAL};
    //orbit only use position
    orbit_model = {orbitBuffer, model::POSITION};
    //Skybox model
    skybox_model = {skyBoxBuffer, model::POSITION};
    //ASSIGNMENT 5
    screenquad_model = {screenQuad, model::POSITION};

    //set starting view
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 10.0f });
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-10.0f), glm::fvec3{ 1.0f, 0.0f, 0.0f });


    //initialisations
    initializeGeometry();
    initializeShaderPrograms();
}

//ASSSIGNMENT 4 - Load all textures for the planets
void ApplicationSolar::loadAllTextures(){

    //get texture
    pixel_data newTexture;

    //load the single texture for every planet
    int textureId;//initialize here to use it later, pass it to the texture parameters
    for(textureId = 0; textureId < NUMBER_OF_CEL_BODIES; textureId++){

        loadSingleTexture(arrayOfPlanets[textureId].name, textureId);

    }

//    loadSkyboxTexture(textureId);
    loadSingleTexture("nebula1", textureId);

}

//ASSIGNMENT 4 - Load a single texture for each planet, based on name and index from the planetTextures
void ApplicationSolar::loadSingleTexture(string name, GLuint id){

    //load texture from file for this planet
    pixel_data newTexture = texture_loader::file(m_resource_path + "textures/" + name + ".png");//converted to .png for better render

    //set ID
    planetTextures[id] = id;

    //switch active texture
//    if(name == "stars"){
//        glActiveTexture((GLenum) (GL_TEXTURE10));
//    }else{
//        glActiveTexture((GLenum) (GL_TEXTURE0 + id));
//    }
    glActiveTexture((GLenum) (GL_TEXTURE0 + id));
    //generate texture object
    glGenTextures(1, &planetTextures[id]);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, planetTextures[id]);

    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //define texture data and texture format

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)newTexture.width, (GLsizei)newTexture.height, 0, newTexture.channels, newTexture.channel_type, newTexture.ptr());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newTexture.width, newTexture.height, 0, newTexture.channels, newTexture.channel_type, newTexture.ptr());

}

//ASSIGNMENT 4 - Load skybox texture
void ApplicationSolar::loadSkyboxTexture(GLuint id) {
    //We re-use the loadSingleTexture here, but to enable/disable the skybox, we made this function separate
    //the only thing that changes is the name of the texture
    //it's not a planet, but it's all the stars
    loadSingleTexture("stars", id + 1);
}

//ASSIGNMENT 4 - Loads the normal map for selected planet
void ApplicationSolar::loadNormalMap(GLenum targetTextureUnit){

    //load texture from file for this planet
    pixel_data newTexture = texture_loader::file(m_resource_path + "textures/earthnormal.png");

    GLuint textureIndex = 11;

    //set ID
    planetTextures[textureIndex] = textureIndex;

    //switch active texture
    glActiveTexture(targetTextureUnit);
    //generate texture object
    glGenTextures(1, &planetTextures[textureIndex]);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, planetTextures[textureIndex]);

    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //define texture data and texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newTexture.width, newTexture.height, 0, newTexture.channels, newTexture.channel_type, newTexture.ptr());
}

//ASSIGNMENT 5
void ApplicationSolar::setupOffscreenRendering(){


    //get screen size
    GLint viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);


    //create texture
    //switch active texture
    glActiveTexture(GL_TEXTURE13);
    //generate texture object
    glGenTextures(1, &drawBufferTexture);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, drawBufferTexture);
    //add empty texture image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportData[2], viewportData[3], 0,
                 GL_RGB, GL_FLOAT, 0);

    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    //create render buffer (for depth buffer)
    glGenRenderbuffers(1, &rbHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, rbHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewportData[2], viewportData[3]);


    //setup FBO
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    //define attachments
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, drawBufferTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, rbHandle);

    //create draw buffers
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);

    //check validity
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE){
        throw std::logic_error("framebuffer error");
    }


}

void ApplicationSolar::render() const {

    //ASSIGNMENT 5
    //set to render to texture (via FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

  //ASSIGNMENT 1
  //RENDER! via a loop
    for (int i = 0; i < NUMBER_OF_CEL_BODIES; i++) {
        if(arrayOfPlanets[i].type != "moon") {
            renderPlanets(arrayOfPlanets[i], i);
        }
    }

    //ASSIGNMENT 4 - render Skybox
    renderSkybox();
    //ASSIGNMENT 2 - render stars and orbits
//    renderStars(); //comment so we can see the skybox
    renderOrbits();
    //ASSIGNMENT 5 - screen quad
    //set to render to texture (via FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderQuad();

}


//ASSIGNMENT 1 - RENDER PLANETS
void ApplicationSolar::renderPlanets(Planet thePlanet, int planetId) const{
    //1st is the already created matrix obj
    //2nd parameter rotation values (deg)
    //3rd parameter is the rotation axis
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * thePlanet.rotation)/10, glm::fvec3{ thePlanet.orbitTilt, 1.0f, 0.0f });

    //VERY IMPORTANT!!!! THE ORDER OF SCALING/ROTATION/TRANSFORMATION WILL AFFECT THE FINAL RESULT AND RENDER!!!!!
    //translate first
    //translate the sphere according to the vector we have in the 2nd param
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0, 0.0, thePlanet.distance});


    //is this planet earth? Then render the moon around it!
    if (thePlanet.name == "earth") {
        //Earth's moon
        int moonId = planetId + 1;
        Planet earthmoon = arrayOfPlanets[moonId];

        //rotation
        glm::fmat4 model_matrix_moon = glm::rotate(model_matrix, float(glfwGetTime() * glm::pi<float>() / 10.0), glm::fvec3{ 1.0f, 0.0f, 0.0f });

        //translation
        model_matrix_moon = glm::translate(model_matrix_moon, glm::fvec3{ 0.0f, 0.0f, earthmoon.distance });
        // scale
        model_matrix_moon = glm::scale(model_matrix_moon, glm::fvec3{ earthmoon.size, earthmoon.size, earthmoon.size});

        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(model_matrix_moon));

        // extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix_moon = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix_moon);
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                           1, GL_FALSE, glm::value_ptr(normal_matrix_moon));

        //ASSIGNMENT 3 - Color for the moon
//        glm::vec3 earthmoonColor = earthmoon.color;
//        glUniform3fv(m_shaders.at("planet").u_locs.at("DiffuseColor"), 1, glm::value_ptr(earthmoonColor));

        //ASSIGNMENT 4 - Add the textures to the planets here
        glActiveTexture((GLenum)(GL_TEXTURE0 + moonId));
        glUniform1i(m_shaders.at("planet").u_locs.at("textureColor"), moonId);

        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);

        // draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    }

    //scale second
    model_matrix = glm::scale(model_matrix, glm::fvec3({thePlanet.size, thePlanet.size, thePlanet.size}));
    //rotate third
	model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * glm::pi<float>() / 10.0), glm::fvec3{ 0.0f, 1.0f, 0.0f });

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));

    //ASSIGNMENT 3 - Render colors here!
    //GEt the color from the struct:
    glm::vec3 planetColor = {thePlanet.color[0]/255, thePlanet.color[1]/255, thePlanet.color[2]/255};
    //Pass it to the shader
    glUniform3fv(m_shaders.at("planet").u_locs.at("DiffuseColor"), 1, glm::value_ptr(planetColor));

    //ASSIGNMENT 3 - Light source: the sun
    //Be brighty bright Sun! This will be the source for the light of the entire system
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    glm::vec4 origin;
    if (thePlanet.name == "sun" ) {
        origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);//initialize origin of light for the sun
    }else {
        origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);//initialize origin of light for the planets

    }
    //multiply by view matrix and upload to shader
    glm::vec3 sunPos(view_matrix * origin);
    glUniform3fv(m_shaders.at("planet").u_locs.at("sunPosition"), 1, glm::value_ptr(sunPos));

    //ASSIGNMENT 4 - Add the textures to the planets here
    glActiveTexture(GL_TEXTURE0 + planetId);
    glUniform1i(m_shaders.at("planet").u_locs.at("textureColor"), planetId);

    //normal map
    glUniform1i(m_shaders.at("planet").u_locs.at("normalMapId"), (int)planetTextures[11]);

    if (thePlanet.name == "earth") {
        glUniform1b(m_shaders.at("planet").u_locs.at("hasBumpMap"), true);
    }
    else {
        glUniform1b(m_shaders.at("planet").u_locs.at("hasBumpMap"), false);
    }

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

}

//ASSIGNMENT 2 - Make the orbit for each planet!
void ApplicationSolar::makeOrbits() {

    //Render the orbit
    for (int i = 0; i < NUMBER_OF_CEL_BODIES; i++) {
        Planet aPlanet = arrayOfPlanets[i];
		////create orbit with diameter 1 and scale before rendering
        int orbit = ORBIT_THICKNESS;
        float counter = 2.0 * glm::pi<float>() / (float)orbit;
        float dist = aPlanet.distance;
        float skew = aPlanet.orbitTilt;
		
		////try a while loop

        for (float orb = 0; orb < (2 * glm::pi<float>()); orb += counter) {
            //x
            orbitBuffer.push_back(dist * cosf(orb));

            //y
            orbitBuffer.push_back(dist * -skew * cosf(orb));
            //z
            orbitBuffer.push_back(dist * sinf(orb));
        }
    }
}

//ASSIGNMENT 2 - Render the orbits
void ApplicationSolar::renderOrbits() const{

    //bind shader and array
    glUseProgram(m_shaders.at("orbit").handle);
    glBindVertexArray(orbit_object.vertex_AO);

    for(int i = 0; i < NUMBER_OF_CEL_BODIES; i++){
        //don't move shader model matrix - orbit is a static loop
        glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(glm::fmat4{}));


        //draw orbit
        glDrawArrays(orbit_object.draw_mode, i * orbit_object.num_elements, orbit_object.num_elements);

        //If the current planet is earth, attach the orbit of the moon to it
        if(arrayOfPlanets[i].name == "earth"){

            Planet earth = arrayOfPlanets[i];
            float earthmoon = earth.size * 0.542;

            //create rotated and translated matrix with planet information
            glm::fmat4 m_earth;

            m_earth = glm::rotate(glm::fmat4{}, float(glfwGetTime() * earth.rotation)/10, glm::fvec3{ 0.0f, 1.0f, 0.0f });
            m_earth = glm::translate(m_earth, glm::fvec3{ 0.0f, 0.0f, earth.distance});
            m_earth = glm::rotate(m_earth, float (M_PI / 1.8f), glm::fvec3{ 0.0f, 0.0f, 1.0f });
            m_earth = glm::scale(m_earth, glm::fvec3{ earthmoon, earthmoon, earthmoon });
            glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
                               1, GL_FALSE, glm::value_ptr(m_earth));
            //draw orbit of the moon (moon's index = i + 1, times the space (10) between the moon and earth)
            glDrawArrays(orbit_object.draw_mode, 50, orbit_object.num_elements);
        }
    }
}

//ASSIGNMENT 2 - Render stars
void ApplicationSolar::renderStars() const{

    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);
    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);
    //draw all
    glDrawArrays(GL_POINTS, 0, star_object.num_elements);

}

//ASSIGNMENT 4 - Render skybox
void ApplicationSolar::renderSkybox() const{

    glUseProgram(m_shaders.at("planet").handle);

    // use rotation speed and planet skew to create planet's orbit
    glm::fmat4 model_matrix;

    // scale skybox so it will be bigger than the solar system here
    float skyboxSize = 70.0f;
    model_matrix = glm::scale(model_matrix, glm::fvec3{skyboxSize, skyboxSize, skyboxSize});

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));

    //extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));


    //textureIndex is last member of planetTextures list, for the skybox
    GLuint textureIndex = 10;

    //Give the star texture to this skybox
//    glActiveTexture(textureIndex);
    glUniform1i(m_shaders.at("planet").u_locs.at("textureColor"), textureIndex);

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);


}

//ASSIGNMENT 5 - render screenquad
void ApplicationSolar::renderQuad() const{

    glUseProgram(m_shaders.at("quad").handle);
    glUniform1i(m_shaders.at("quad").u_locs.at("TexID"), drawBufferTexture);
    glUniform1i(m_shaders.at("quad").u_locs.at("PP_FLAG"), Post_Processing_Flag);

    glBindVertexArray(screenquad_object.vertex_AO);
    glDrawArrays(screenquad_object.draw_mode, 0, screenquad_object.num_elements);

}


void ApplicationSolar::updateView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);

    glUseProgram(m_shaders.at("planet").handle);

    //ASSIGNMENT 3 - light source for sun and planets
    //added for assignment 3 - upload sun's position to planet shader
    //create vec 4 of light origin (sun)
    glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
    //multiply by view matrix and upload to shaders
    glm::vec3 sunPos(view_matrix * origin);
    glUniform3fv(m_shaders.at("planet").u_locs.at("sunPosition"), 1, glm::value_ptr(sunPos));


    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

    //ASSIGNMENT 2 - For orbit and stars
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
    // upload matrix to gpu

    glUseProgram(m_shaders.at("planet").handle);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));

    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));

    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));


    //update render buffer size
    glBindRenderbuffer(GL_RENDERBUFFER, rbHandle);
    //get screen size
    GLint viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, viewportData[2], viewportData[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportData[2], viewportData[3], 0,
                 GL_RGB, GL_FLOAT, 0);
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();

  // bind new shader
  glUseProgram(m_shaders.at("planet").handle);

  updateView();
  updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
    //move scene toward camera
    if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.3f});
        updateView();
    }
        // move scene away from camera
    else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.3f});
        updateView();
    }
        // move scene right
    else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -1.0f, 0.0f, 0.0f });
        updateView();
    }
        //move scene left
    else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 1.0f, 0.0f, 0.0f });
        updateView();
    }

    //ASSIGNMENT 3 - Switching using the keyboard
    //switch between shading modes - mode 1
    else if (key == GLFW_KEY_1 && action != GLFW_PRESS) {

        glUseProgram(m_shaders.at("planet").handle);
        glUniform1i(m_shaders.at("planet").u_locs.at("ShaderMode"), 1);

    }
        //switch between shading modes - mode 2
    else if (key == GLFW_KEY_2 && action != GLFW_PRESS) {
        glUseProgram(m_shaders.at("planet").handle);
        glUniform1i(m_shaders.at("planet").u_locs.at("ShaderMode"), 2);
    }

    //ASSIGNMENT 5
    //map flags on keys, taken from here:
    //https://www.experts-exchange.com/articles/1842/Binary-Bit-Flags-Tutorial-and-Usage-Tips.html

    else if (key == GLFW_KEY_7 && action != GLFW_PRESS){
        Post_Processing_Flag ^= 1UL << 0;//greyscale
    }
    else if (key == GLFW_KEY_8 && action != GLFW_PRESS){
        Post_Processing_Flag ^= 1UL << 1;//horizontal flip
    }
    else if (key == GLFW_KEY_9 && action != GLFW_PRESS){
        Post_Processing_Flag ^= 1UL << 2;//vertical flip
    }
    else if (key == GLFW_KEY_0 && action != GLFW_PRESS){
        Post_Processing_Flag ^= 1UL << 3;//blur
    }

}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
    // mouse handling
    float x = 0.0f;
    float y = 0.0f;

    //move the camera according to the mouse
    const float moveMouseX = 0.9f;
    const float moveMouseY = 0.3f;
    //    Move in X
    if (pos_x > 0) {
        x = -moveMouseX;
    }
    else if (pos_x < 0) {
        x = moveMouseX;
    }
    //    Move in Y
    if (pos_y > 0) {
        y = -moveMouseY;
    }
    else if (pos_y < 0) {
        y = moveMouseY;
    }
    // If the rotation of the mouse changes, you should too

    m_view_transform = glm::rotate(m_view_transform, glm::radians(x), glm::fvec3{ 0.0f, 1.0f, 0.0f });
    m_view_transform = glm::rotate(m_view_transform, glm::radians(y), glm::fvec3{ 1.0f, 0.0f, 0.0f });

    updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
    // store shader program objects in container
    m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                               m_resource_path + "shaders/simple.frag"});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

    //ASSIGNMENT 3 - Add source of light, diffuse color, and shader mode
    m_shaders.at("planet").u_locs["sunPosition"] = -1;
    m_shaders.at("planet").u_locs["DiffuseColor"] = -1;
    m_shaders.at("planet").u_locs["ShaderMode"] = -1;
    //ASSIGNMENT 4 - Textures
    m_shaders.at("planet").u_locs["textureColor"] = -1;
    m_shaders.at("planet").u_locs["normalMapId"] = -1;
    m_shaders.at("planet").u_locs["hasBumpMap"] = -1;


    // add star shader here
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
                                             m_resource_path + "shaders/star.frag"});
    // request uniform locations for shader program
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

    // add orbit shader here
    m_shaders.emplace("orbit", shader_program{m_resource_path + "shaders/orbit.vert",
                                              m_resource_path + "shaders/orbit.frag"});
    // request uniform locations for shader program
    m_shaders.at("orbit").u_locs["ModelMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

    //ASSIGNMENT 5
    m_shaders.emplace("quad", shader_program{m_resource_path + "shaders/quad.vert",
                                             m_resource_path + "shaders/quad.frag"});
    m_shaders.at("quad").u_locs["Texture"] = -1;
    m_shaders.at("quad").u_locs["TexID"] = -1;
    m_shaders.at("quad").u_locs["PP_FLAG"] = -1;
    //test
//    m_shaders.at("quad").u_locs["Color"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {

    //=================================================================
    // planet initialisation

    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);

    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

    // activate third attribute on gpu - texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

    // activate fourth attribute on gpu - tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, model::TANGENT.components, model::TANGENT.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TANGENT]);



    // generate generic buffer
    glGenBuffers(1, &planet_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

    // store type of primitive to draw
    planet_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    planet_object.num_elements = GLsizei(planet_model.indices.size());


    //======================================================================
    // star initialisation - lecture 4 slide 8


    //generate vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * star_model.data.size(), star_model.data.data(), GL_STATIC_DRAW);



    // activate first attribute on gpu - position
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride

    //reinstate
    glVertexAttribPointer(0, star_model.POSITION.components, star_model.POSITION.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::POSITION]);


    // activate second attribute on gpu - colour
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with offset & stride of 3 floats

    glVertexAttribPointer(1, star_model.NORMAL.components, star_model.NORMAL.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::NORMAL]);


    // end star initialisation
    //======================================================================
    //orbit initialisation


    // generate vertex array object
    glGenVertexArrays(1, &orbit_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(orbit_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &orbit_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orbit_model.data.size(), orbit_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride

    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, orbit_model.vertex_bytes, orbit_model.offsets[model::POSITION]);

    // store type of primitive to draw
    orbit_object.draw_mode = GL_LINE_LOOP;

    //ASSIGNMENT 5

    // generate vertex array object
    glGenVertexArrays(1, &screenquad_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(screenquad_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &screenquad_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, screenquad_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenquad_model.data.size(), screenquad_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, screenquad_model.vertex_bytes, screenquad_model.offsets[model::POSITION]);


    // transfer number of indices to model object
    screenquad_object.num_elements = GLsizei(screenquad_model.data.size());
    screenquad_object.draw_mode = GL_TRIANGLE_STRIP;

}

//returns a position float, up to specified max value
//assigns random sign to value (-/+)
float ApplicationSolar::randPos(){

    float maxDistFromCentre = 80.0;

    //multiply and divide by factor of 10 to give more precision
    int range = int (maxDistFromCentre * 10);

    float out = float(rand() % range) / 10.f;

    //assign random sign - make value negative if rand num is even
    if (rand() % 2 == 0){
        out = -out;
    }

    return out;
}

//returns a colour between 0.0 and 1.0;
float ApplicationSolar::randCol(){
    float out = float(rand() % 100) / 100.0f;
    return out;
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  //ASSIGNMENT 2 - DESTROY EVERYTHING! this is the destructor anyway
  //Destroy for garbage collection, C++ ain't Java
  //delete stars
  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  //delete orbits
  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);

  //ASSIGNMENT 5
  //delete quad buffers
  glDeleteBuffers(1, &screenquad_object.vertex_BO);
  glDeleteVertexArrays(1, &screenquad_object.vertex_AO);
  //delete render buffer
  glDeleteRenderbuffers(1, &rbHandle);
  //delete framebuffer
  glDeleteFramebuffers(1, &fboHandle);
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}