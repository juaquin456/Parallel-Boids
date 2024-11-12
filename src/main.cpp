#include <iostream>
#include "Quadtree.h"
#include "Boid.h"

using namespace std;

int main() {
  vector<shared_ptr<Boid>> boids;
  quadtree::Box<float> universe{0., 100, 100, 100};
  quadtree::Quadtree<shared_ptr<Boid>, decltype(&Boid::getBox)> boids_tree(universe, Boid::getBox);

  for (const auto& boid: boids) {
    boids_tree.add(boid);
  }

  float step = 2.;
  while (true) {
    for (auto &boid : boids) {
      boid->updatePosition(step, boids_tree);
    }
    //boids_tree.update(); or
    //boids_tree =

    // render()
  }

  return 0;
}
