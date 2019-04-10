// Should forces be smoothed out?
// How to split around sphere with using force applied along normal rather than tangent vector
// "initial states and parameters of the simulations should be read from a text data file, rather than hard-coded"?
// "Write a short manual explaining the operation of your program from a user’s perspective"?


//Bonus 1: Increase number of boids in real time
//Bonus 2: Provide the ability to guide the flocks

#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>

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

int numberOfBoids = 200;
std::vector<Boid> boids;

vec3 sphereOrigin = vec3(20.0f, 0.0f, 0.0f);
float sphereRadius = 4;

float ra = 3;       //Radius avoid
float rc = 10;       //Radius cohesion
float rg = 20;      //Radius gathering
float rMax = 20;    //Radius of simulation area

float minVelocity = 10;
float maxVelocity = 20;

void updateBoids();


//////////////////////////////////////////////////////////////////
////////////////////////// Scene Setup ///////////////////////////
//////////////////////////////////////////////////////////////////

int main(void) {
    //Window setup
  srand(time(NULL));
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Simple example");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);




  //Create meshs
  auto phongStyle = Phong(
      LightPosition(0.0, -100.0, 0.0),
      Colour(1.0, 1.0, 0.1529)
  );

  auto boidMesh = Mesh(Filename("./models/goose.obj"));
  auto instancedBoid = createInstancedRenderable(boidMesh, phongStyle);

  //For cylinder use just XY and alighn cyclinder along z

  auto instancedSpheres = createInstancedRenderable(
              Sphere(Centroid(sphereOrigin.x, sphereOrigin.y, sphereOrigin.z), Radius(sphereRadius)), phongStyle);


  for (int i = 0 ; i < numberOfBoids ; i++){
      Boid newBoid = Boid();

      //Randomize start positions
      float startBound = pow((pow(rMax,2)/2),0.5);
      float xStart = (rand() % (2*(int)startBound)) - startBound;
      float yStart = (rand() % (2*(int)startBound)) - startBound;
      float zStart = (rand() % (2*(int)startBound)) - startBound;

      newBoid.position = vec3(xStart, yStart, zStart);

      //Randomize start direction
      float xVel = (rand() % (2*(int)startBound)) - startBound;
      float yVel = (rand() % (2*(int)startBound)) - startBound;
      float zVel = (rand() % (2*(int)startBound)) - startBound;

      vec3 randomVel = normalize(vec3(xVel, yVel, zVel));
      newBoid.velocity = randomVel*vec3(minVelocity,minVelocity,minVelocity);

      boids.push_back(newBoid);
      //std::cout << "Boid created at x = " << xStart << std::endl;
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
        //Calculate boid-boid forces
        for (int j = 0 ; j < numberOfBoids ; j++){
            if (i != j){
                vec3 toBoidVec = boids[j].position - boids[i].position;
                float dist = length(toBoidVec);

                if(dist < ra){
                    float c = pow(ra,2)/pow(dist,2);
                    boids[i].totalForce -= normalize(toBoidVec) * vec3(c, c, c);
                }

                else if(dist < rc){
                    vec3 aveVec = (boids[i].velocity+boids[j].velocity)/vec3(2.0f, 2.0f, 2.0f);
                    boids[i].totalForce += aveVec;
                }

                else if(dist < rg){
                    float c = 0.5f;
                    boids[i].totalForce += normalize(toBoidVec) * vec3(c, c, c);
                }
            }
        }

        //Calculate boid-object forces
        for (int j = 0 ; j < numberOfBoids ; j++){
            //Sphere collision
            vec3 point = boids[i].position - sphereOrigin;
            vec3 v =  boids[i].velocity;
            float a = dot(v,v);                                  //v.v
            float b = 2*dot(point, v);                           //2X`.v
            float c = dot(point, point) - pow(sphereRadius,2);   //X`.X` - r^2

            float quadPart = pow(b,2)-(4*a*c);
            if(quadPart >= 0){                  //If collision
                float t1 = (-b + pow(quadPart, 0.5))/(2*a);
                float t2 = (-b - pow(quadPart, 0.5))/(2*a);

                float pt = -1.0f;
                if(t1 >= 0 && t2 >= 0)
                    pt = min(t1, t2);
                else if (t1 >= 0 && t2 < 0)
                    pt = t1;
                else if (t1 < 0 && t2 >= 0)
                    pt = t2;

                if(pt >= 0){
                    vec3 p = boids[i].position + vec3(v.x*pt, v.y*pt, v.z*pt);
                    vec3 forceVec = normalize(p - sphereOrigin);
                    float forceScale = 20/pow(length(boids[i].position-p),2);
                    boids[i].totalForce += forceVec * vec3(forceScale, forceScale, forceScale);
                }
            }
        }

        //Calculate boid-boundry forces
        vec3 toCenterVec = vec3(0.0f, 0.0f, 0.0f) - boids[i].position; //Vector from boid to center
        if(length(toCenterVec) > rMax){
            float centerForce = pow((length(toCenterVec)-rMax), 2);
            boids[i].totalForce += normalize(toCenterVec) * centerForce;
        }

        //Lure force
        //vec3 lurePosition = vec3(0.0f, 0.0f, 0.0f);
        //float lureStrength = 20.0f;
        //boids[i].totalForce += normalize(lurePosition - boids[i].position) * vec3(lureStrength, lureStrength, lureStrength);


        //ALL FORCES HAVE BEEN ADDED BY THIS LINE

        vec3 V = normalize(boids[i].velocity);
        vec3 n = boids[i].totalForce - g;
        vec3 B = normalize(cross(V,n));
        vec3 N = normalize(cross(B,V));
        vec3 P = boids[i].position;

        auto m = mat4f{ {B.x, B.y, B.z, 0.0f},
                        {N.x, N.y, N.z, 0.0f},
                        {V.x, V.y, V.z, 0.0f},
                        {P.x, P.y, P.z, 1.0f}};

        addInstance(instancedBoid, m);
    }

    draw(instancedBoid, view);

    addInstance(instancedSpheres, mat4f{1.f});
    draw(instancedSpheres, view);

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
        boids[i].velocity = boids[i].velocity + (a*deltT);

        //Impose basic limits on velocities
        if(length(boids[i].velocity) < minVelocity)
            boids[i].velocity = normalize(boids[i].velocity) * vec3(minVelocity, minVelocity, minVelocity);
        if(length(boids[i].velocity) > maxVelocity)
            boids[i].velocity = normalize(boids[i].velocity) * vec3(maxVelocity, maxVelocity, maxVelocity);



        boids[i].position = boids[i].position + (boids[i].velocity*deltT);
        //std::cout << "Position updated to (" << boids[i].position.x << "," <<  boids[i].position.y << "," << boids[i].position.z << ")" << std::endl;
        boids[i].totalForce = vec3(0.0f, 0.0f, 0.0f);
    }

}
