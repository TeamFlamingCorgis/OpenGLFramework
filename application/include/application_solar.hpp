#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "planet.h" //ASSIGNMENT 1

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
    //Render planets
    void renderPlanets(Planet thePlanet) const;
    //Assignment 2 - Make stars and orbits
    void makeStars();
    void renderStars() const;
    void makeOrbits(Planet aPlanet);
    void renderOrbits() const;

    //Assignment 2 - buffers for star and orbit
    std::vector<float> orbitBuffer;
    std::vector<float> starBuffer;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;

  //Assignment 2 - Star model and orbit model
  model_object star_object;
  model_object orbit_object;
};

#endif