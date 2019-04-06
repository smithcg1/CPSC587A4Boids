//------------------------------------------------------------------------------
// A simple example showing how to use the triangle geometry
//------------------------------------------------------------------------------

//Do we want gravity?
//Should I use the graphs?
//Need boid object interaction?

#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>

#include "io.h"
#include "panel.h"
#include "turntable_controls.h"

#include "Boid.h"

using namespace glm;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;

float deltT = 0.01f;
const vec3 g = vec3{0.0f, -9.8f, 0.0f};

int numberOfBoids = 5;
std::vector<Boid> boids;

float ra = 3;       //Radius avoid
float rc = 6;       //Radius cohesion
float rg = 12;      //Radius gathering
float rMax = 20;    //Radius of simulation area

void updateBoids();


//////////////////////////////////////////////////////////////////
////////////////////////// Scene Setup ///////////////////////////
//////////////////////////////////////////////////////////////////

int main(void) {
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Simple example");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);

  //Create mesh for boids
  auto p1 = vec3f(-0.5f, 0.0f,  0.0f);
  auto p2 = vec3f( 0.0f, 0.0f, -1.0f);
  auto p3 = vec3f( 0.5f, 0.0f,  0.0f);

  auto instancedTriangle = createInstancedRenderable(
      Triangle(Point1(p1), Point2(p2), Point3(p3)),
      Phong(Colour(1., 1., 0.1529), LightPosition(0.0f, 100.f, 0.0f)));




  for (int i = 0 ; i < numberOfBoids ; i++){
      Boid newBoid = Boid();
      newBoid.position = vec3(i*2.0f, 0.0f, 0.0f);
      boids.push_back(newBoid);
      std::cout << "Boid created" << std::endl;
  }





  //////////////////////////////////////////////////////////////////
  ///////////////////////// Rendering loop /////////////////////////
  //////////////////////////////////////////////////////////////////
  ///
  window.run([&](float frameTime) {

    glfwPollEvents();
    p::menu();

    auto color = p::clear_color;
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view.projection.updateAspectRatio(window.width(), window.height());

    //Physics calculations
    updateBoids();

    for (int i = 0 ; i < numberOfBoids ; i++){
        auto m = translate(mat4f{1.f}, boids[i].position);
        addInstance(instancedTriangle, m);

        //Calculate boid-boid forces
        for (int j = 0 ; j < numberOfBoids ; j++){
            if (i != j){






            }
        }

        //Calculate boid-object forces
        for (int j = 0 ; j < numberOfBoids ; j++){

        }

        //Calculate boid-limit forces
        vec3 toCenterVec = vec3(0.0f, 0.0f, 0.0f) - boids[i].position; //Vector from boid to center
        if(length(toCenterVec) > rMax)
            boids[i].totalForce += toCenterVec;
    }

    draw(instancedTriangle, view);

    io::renderDrawData();
  });
  exit(EXIT_SUCCESS);
}


//////////////////////////////////////////////////////////////////
///////////////////////// Physics stuff //////////////////////////
//////////////////////////////////////////////////////////////////

void updateBoids(){
    for (int i = 0 ; i < boids.size() ; i++){
        vec3 a = boids[i].totalForce;
        //a += g;

        boids[i].velocity = boids[i].velocity + (a*deltT);
        boids[i].position = boids[i].position + (boids[i].velocity*deltT);
        //std::cout << "Position updated to (" << boids[i].position.x << "," <<  boids[i].position.y << "," << boids[i].position.z << ")" << std::endl;
        boids[i].totalForce = vec3(0.0f, 0.0f, 0.0f);
    }

}
