#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

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

//Planet sun, mercury, venus, earth, earthmoon, mars, jupiter, saturn, uranus, neptune;
////gather all the planets in an array
//Planet arrayOfPlanets[NUMBER_OF_CEL_BODIES] = { sun, mercury, venus, earth, earthmoon, mars, jupiter, saturn, uranus, neptune };

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 , planet_object{}, star_object{}, orbit_object{}
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


    //MODELS GO HERE
    //planet
//    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
    //stars only use colors
    star_model = {starBuffer, model::POSITION | model::NORMAL};
    //orbit only use position
    orbit_model = {orbitBuffer, model::POSITION};


  initializeGeometry();
  initializeShaderPrograms();

  //set starting view
  m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 2.0f, 20.0f });
  m_view_transform = glm::rotate(m_view_transform, glm::radians(-10.0f), glm::fvec3{ 1.0f, 0.0f, 0.0f });


}

void ApplicationSolar::render() const {

    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

  //ASSIGNMENT 1
  //RENDER! via a loop
    for (int i = 0; i < NUMBER_OF_CEL_BODIES; i++) {
        renderPlanets(ApplicationSolar::arrayOfPlanets[i]);
    }

    //ASSIGNMENT 2
    renderStars();
    renderOrbits();

}


//ASSIGNMENT 1 - RENDER PLANETS
void ApplicationSolar::renderPlanets(Planet thePlanet) const{
    //1st is the already created matrix obj
    //2nd parameter rotation values (deg)
    //3rd parameter is the rotation axis
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * thePlanet.rotation), glm::fvec3{ 0.0f, 1.0f, 0.0f });

    //    VERY IMPORTANT!!!! THE ORDER OF SCALING/ROTATION/TRANSFORMATION WILL AFFECT THE FINAL RESULT AND RENDER!!!!!
    //translate first
    //translate the sphere according to the vector we have in the 2nd param
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0, 0.0, thePlanet.distance});


    //is this planet earth? Then render the moon around it!
    if (thePlanet.type == "moon") {
        //Earth's moon
        Planet earthmoon = thePlanet;

        //rotation
        glm::fmat4 model_matrix_moon = glm::rotate(model_matrix, float(glfwGetTime() * earthmoon.rotation), glm::fvec3{ 1.0f, 0.0f, 0.0f });

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

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

}

//ASSIGNMENT 2 - Make the orbit for each planet!
void ApplicationSolar::makeOrbits() {

    //If it's not the sun, render the orbit
    for (int i = 1; i < NUMBER_OF_CEL_BODIES; i++) {
        Planet aPlanet = ApplicationSolar::arrayOfPlanets[i];

        int orbit = ORBIT_THICKNESS;
        float counter = 2.0 * glm::pi<float>() / (float)orbit;
        float dist = aPlanet.distance;

        for (float orb = 0; orb < (2 * glm::pi<float>()); orb += counter) {
            //x
            orbitBuffer.push_back(dist * cosf(orb));
            //y
            orbitBuffer.push_back(dist * -0.3 * cosf(orb));
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

    //render every orbit
    for (int i = 1; i < NUMBER_OF_CEL_BODIES; i++) {
        Planet aPlanet = arrayOfPlanets[i];

        //render the orbits for the planets first
        if (aPlanet.type != "moon") {

            //don't move shader model matrix - orbit is a static loop
            glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
                               1, GL_FALSE, glm::value_ptr(glm::fmat4{}));


            //draw orbit
            glDrawArrays(orbit_object.draw_mode, i * orbit_object.num_elements, orbit_object.num_elements);

            //if this planet has a moon
            if (arrayOfPlanets[i].name == "earth") {

                Planet earth = arrayOfPlanets[i];

                //create rotated and translated matrix with planet information
                glm::fmat4 m_earth = glm::rotate(glm::fmat4{}, float(glfwGetTime() * earth.rotation), glm::fvec3{ 0.0f, 1.0f, 0.0f });
                m_earth = glm::translate(m_earth, glm::fvec3{ 0.0f, 0.0f, earth.rotation });
                m_earth = glm::rotate(m_earth, float (M_PI / 2.f), glm::fvec3{ 0.0f, 0.0f, 1.0f });

                //update shader model matrix
                glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
                                   1, GL_FALSE, glm::value_ptr(m_earth));

                //draw orbit
                glDrawArrays(orbit_object.draw_mode, i * 100, orbit_object.num_elements);

            }
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


void ApplicationSolar::updateView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
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
}

// load models
void ApplicationSolar::initializeGeometry() {
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


    //ASSIGNMENT 2 - Stars
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



    //ASSIGNMENT 2 - Orbits
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
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}