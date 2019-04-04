//------------------------------------------------------------------------------
// A simple example showing how to use the triangle geometry
//------------------------------------------------------------------------------

//Do we want air resistance?
//Do we want gravity?


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
const vec3 g = vec3{0.0f, 9.8f, 0.0f};
std::vector<Boid> boids;

void updateBoids();


int main(void) {
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Simple example");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);

  auto instancedSphere = createInstancedRenderable(
      Sphere(Centroid(0.0, 1., 0.)),
      Phong(Colour(1., 1., 0.1529), LightPosition(100.f, 100.f, 100.f)));


  Boid testBoid = Boid();
  boids.push_back(testBoid);

  auto p1 = vec3f(boids[0].position.x, boids[0].position.y, boids[0].position.z +0.1f);
  auto p2= vec3f(boids[0].position.x +0.2f, boids[0].position.y, boids[0].position.z);
  auto p3 = vec3f(boids[0].position.x, boids[0].position.y, boids[0].position.z -0.1f);

  auto instancedTriangle = createInstancedRenderable(
      Triangle(Point1(p1), Point2(p2), Point3(p3)),
      Phong(Colour(1., 1., 0.1529), LightPosition(0.0f, 100.f, 0.0f)));



  std::vector<float> times;
  times.push_back(0.f);

  window.keyboardCommands() |
      io::Key(GLFW_KEY_P, [](auto) { p::showPanel = true; });

  auto xFunc = p::funcs.create("x Value", 20);
  auto yFunc = p::funcs.create("y Value", 20);

  window.run([&](float frameTime) {

    glfwPollEvents();
    p::menu();

    if (p::addBall) {
      times.push_back(0.f);
      p::ballCount = times.size();
    }

    auto color = p::clear_color;
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view.projection.updateAspectRatio(window.width(), window.height());

    for (auto &t : times) {
      t += deltT;
      t = t < 1.f ? t : 0.f;

      float x = p::funcs.evaluateFast(xFunc, t);
      x = glm::mix(p::xRange[0], p::xRange[1], x);
      float y = p::funcs.evaluateFast(yFunc, t);
      y = glm::mix(p::yRange[0], p::yRange[1], y);
      mat4f m = translate(mat4f(1.f), vec3f{x, y, 0.0});
      m = scale(m, 15.f * vec3f{p::scale});

      //addInstance(instancedSphere, m);
      addInstance(instancedTriangle, m);
    }

    //draw(instancedSphere, view);
    draw(instancedTriangle, view);

    io::renderDrawData();

  });
  exit(EXIT_SUCCESS);
}



void updateBoids(){
    for (int i = 0 ; i < boids.size() ; i++){
        vec3 a = boids[i].totalForce;
        a += g;

        boids[i].velocity = boids[i].velocity + (a*deltT);
        boids[i].position = boids[i].position + (boids[i].velocity*deltT);
        boids[i].totalForce = vec3(0.0f, 0.0f, 0.0f);
    }

}
