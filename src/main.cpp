#include "Quadtree.h"
#include "Boid.h"

#include "SFML/Graphics.hpp"

using namespace std;
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Boids");
  vector<shared_ptr<Boid>> boids;
  for(uint i = 0; i < 100; i++) {
    float x = rand() % WIDTH;
    float y = rand() % HEIGHT;
    boids.emplace_back(make_shared<Boid>(Point{x, y}, Point{1, 0}));
  }

  quadtree::Box<float> universe{0., 0, WIDTH, HEIGHT};
  quadtree::Quadtree<shared_ptr<Boid>, decltype(&Boid::getBox)> boids_tree(universe, Boid::getBox);

  for (const auto& boid: boids) {
    boids_tree.add(boid);
  }

  float step = 2.;
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }
    for (auto &boid : boids) {
      boid->updatePosition(step, boids_tree);
    }
    //boids_tree.update(); or
    //boids_tree =
    window.clear();
    for (const auto& boid: boids) {
      boid->draw(window);
    }
    window.display();
  }

  return 0;
}
