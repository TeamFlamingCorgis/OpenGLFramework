#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "planet.h" //ASSIGNMENT 1
//Define planets and moon
#define  NUMBER_OF_CEL_BODIES 10
#define  DEFAULT_SIZE 0.4 //Default sun size
//Declare defaults for the min distance from the sun, the default for rotation, orbit, and speed time
#define  DEFAULT_DIST 0.4 * 20.0
#define  DEFAULT_ROTATION 0.5

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();
  //ASSIGNMENT 1
  //Render planets
  void renderPlanets(Planet thePlanet) const;
  //ASSIGNMENT 2 - render stars and orbits
  void renderStars() const;
  void renderOrbits() const;


    std::vector< float > starBuffer;
    std::vector< float > orbitBuffer;
    void makeOrbits();
    float randPos();
    float randCol();

    // cpu representation of model
    model_object planet_object;
    //ASSIGNMENT 2 - model objects of stars and orbits
    model_object star_object;
    model_object orbit_object;

  //ASSIGNMENT 1 - MOVED HERE TO MAKE IT GLOBAL!!!
  Planet arrayOfPlanets[NUMBER_OF_CEL_BODIES] ={
          {
            //SUN
            "sun", //name
            DEFAULT_SIZE * 6.0f,
            0.0f, //stays in the center, doesnt have to move
            0.0f, //for the rotation time on renderPlanet
            "sun", //are you a sun, a planet, or a star?
            {184,239,52} //color from rgb: thisColor = rgbValue / 255 (to fit the req from OpenGL)
          },
          {
            //MERCURY
            "mercury", //name
            DEFAULT_SIZE * 0.383f,
            DEFAULT_DIST * 0.387f,
            DEFAULT_ROTATION * 0.59f,
            "planet",
            {184,239, 52}
          },
          {
            //VENUS
            "mercury", //name
            DEFAULT_SIZE * 0.949f,
            DEFAULT_DIST * 0.723f,
            DEFAULT_ROTATION * 0.24f,
            "planet",
            {184,239, 52}
          },
          {
            //EARTH
            "earth", //name
            DEFAULT_SIZE,
            DEFAULT_DIST,
            DEFAULT_ROTATION,
            "planet",
            {184,239, 52}
          },
          {
            //EARTH'S MOON
            "earthmoon", //name
            DEFAULT_SIZE * 0.0272f,
            DEFAULT_DIST * 0.08f,
            DEFAULT_ROTATION * 0.027f,
            "moon",
            {184,239, 52}
          },
          {
            //MARS
            "mars", //name
            DEFAULT_SIZE * 0.532f,
            DEFAULT_DIST * 1.52f,
            DEFAULT_ROTATION * 1.03f,
            "planet",
            {184,239, 52}
          },
          {
            //JUPITER
            "jupiter", //name
            DEFAULT_SIZE * 6.21f,
            DEFAULT_DIST * 3.2f,
            DEFAULT_ROTATION * 0.95f,
            "planet",
            {184,239, 52}
          },
          {
            //SATURN
            "saturn",
            DEFAULT_SIZE * 6.45f,
            DEFAULT_DIST * 4.58f,
            DEFAULT_ROTATION * 0.9f,
            "planet",
            {184,239, 52}
          },
          {
            //URANUS
            "uranus",
            DEFAULT_SIZE * 3.01f,
            DEFAULT_DIST * 5.2f,
            DEFAULT_ROTATION * 0.85f,
            "planet",
            {184,239, 52}
          },
          {
            //NEPTUNE
            "neptune",
            DEFAULT_SIZE * 2.88f,
            DEFAULT_DIST * 6.05f,
            DEFAULT_ROTATION * 0.8f,
            "planet",
            {184,239, 52}
          }
  };
};

#endif