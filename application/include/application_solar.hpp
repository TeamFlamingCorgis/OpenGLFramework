#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "planet.h" //ASSIGNMENT 1
//Define planets and moon
#define  NUMBER_OF_CEL_BODIES 10
#define  DEFAULT_SIZE 0.4f //Default sun size
//Declare defaults for the min distance from the sun, the default for rotation, orbit, and speed time
#define  DEFAULT_DIST 0.4f * 20.0f
#define  DEFAULT_ROTATION 0.5f

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
  void renderPlanets(Planet thePlanet, int planetId) const;
  //ASSIGNMENT 2 - render stars and orbits
  void renderStars() const;
  void renderOrbits() const;
  void renderSkybox() const;

private:
    //ASSIGNMENT 2 - Make the orbits
    void makeOrbits();
    float randPos();
    float randCol();
    //ASSIGNMENT 4 - load the textures
    void loadAllTextures();
    void loadSingleTexture(std::string name, GLuint id);
    void loadSkyboxTexture( GLuint id);
    void loadNormalMap(GLenum targetTextureUnit);


    //ASSIGNMENT 2 - stars and orbit buffer
    std::vector< float > starBuffer;
    std::vector< float > orbitBuffer;
    //ASSIGNMENT 4 - add Skybox buffer
    std::vector< float > skyBoxBuffer;

    //ASSIGNMENT 4 - Array for the textures
    GLuint planetTextures[NUMBER_OF_CEL_BODIES + 2];

    // cpu representation of model
    model_object planet_object;
    //ASSIGNMENT 2 - model objects of stars and orbits
    model_object star_object;
    model_object orbit_object;
    model_object skybox_object;

    //ASSIGNMENT 1 - MOVED HERE TO MAKE IT GLOBAL!!!
    Planet arrayOfPlanets[NUMBER_OF_CEL_BODIES] ={
            {
                    //SUN
                    "sun", //name
                    DEFAULT_SIZE * 6.0f,
                    0.0f, //stays in the center, doesnt have to move
                    0.0f, //for the rotation time on renderPlanet
                    0.0f, //the orbit tilt of the planet, for the how the planet renders in the orbit
                    "sun", //are you a sun, a planet, or a star?
                    {252,252,200} //color from rgb: thisColor = rgbValue / 255 (to fit the req from OpenGL)
            },
            {
                    //MERCURY
                    "mercury", //name
                    DEFAULT_SIZE * 0.383f,
                    DEFAULT_DIST * 0.387f,
                    DEFAULT_ROTATION * 0.57f,
                    0.2f,
                    "planet",
                    {231,0,0}
            },
            {
                    //VENUS
                    "venus", //name
                    DEFAULT_SIZE * 0.949f,
                    DEFAULT_DIST * 0.723f,
                    DEFAULT_ROTATION * 0.20f,
                    -0.2f,
                    "planet",
                    {255,140,0}
            },
            {
                    //EARTH
                    "earth", //name
                    DEFAULT_SIZE,
                    DEFAULT_DIST,
                    DEFAULT_ROTATION,
                    0.0f,
                    "planet",
                    {12,187,230}
            },
            {
                    //EARTH'S MOON
                    "earthmoon", //name
                    DEFAULT_SIZE * 0.178f,
                    DEFAULT_DIST * 0.0844f,
                    DEFAULT_ROTATION * 0.025f,
                    0.0f,
                    "moon",
                    {112,110,110}
            },
            {
                    //MARS
                    "mars", //name
                    DEFAULT_SIZE * 0.532f,
                    DEFAULT_DIST * 1.52f,
                    DEFAULT_ROTATION * 1.00f,
                    0.03f,
                    "planet",
                    {255,239,0}
            },
            {
                    //JUPITER
                    "jupiter", //name
                    DEFAULT_SIZE * 6.21f,
                    DEFAULT_DIST * 3.2f,
                    DEFAULT_ROTATION * 0.92f,
                    -0.1f,
                    "planet",
                    {0,129,31}
            },
            {
                    //SATURN
                    "saturn",
                    DEFAULT_SIZE * 6.45f,
                    DEFAULT_DIST * 5.58f,
                    DEFAULT_ROTATION * 0.87f,
                    0.05f,
                    "planet",
                    {118,0,137}
            },
            {
                    //URANUS
                    "uranus",
                    DEFAULT_SIZE * 3.01f,
                    DEFAULT_DIST * 6.2f,
                    DEFAULT_ROTATION * 0.82f,
                    -0.05f,
                    "planet",
                    {0,68,255}
            },
            {
                    //NEPTUNE
                    "neptune",
                    DEFAULT_SIZE * 2.88f,
                    DEFAULT_DIST * 7.05f,
                    DEFAULT_ROTATION * 0.78f,
                    -0.02f,
                    "planet",
                    {251,4,75}
            }
    };
};

#endif