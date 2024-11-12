#include <iostream>
#include "Quadtree.h"
#include "Boid.h"

#include "SFML/Graphics.hpp"

using namespace std;
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Boids");
  vector<shared_ptr<Boid>> boids;
  vector<Boid::Interface> interfaces;
  for(int i = 0; i < 100; i++) {
    float x = rand() % WIDTH;
    float y = rand() % HEIGHT;
    boids.emplace_back(make_shared<Boid>(Point{x, y}, Point{1, 0}));
    interfaces.push_back(Boid::getInterface(boids[i]));
  }

  quadtree::Box<float> universe{0., 0, WIDTH, HEIGHT};
  quadtree::Quadtree<Boid::Interface, decltype(&Boid::Interface::getBox)> boids_tree(universe, Boid::Interface::getBox);

  for (const auto& boidref: interfaces) {
    boids_tree.add(boidref);
  }

  sf::Clock clock;
  while (window.isOpen()) {
    sf::Time elapsed = clock.restart();
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }

    for (auto &boid : boids) {
      boid->updatePosition(elapsed.asSeconds(), boids_tree);
    }

    for (auto &boid: interfaces) {
      boids_tree.remove(boid);
      boid.update();
      boids_tree.add(boid);
    }

    // redraw
    window.clear();
    for (const auto& boid: boids) {
      boid->draw(window);
    }
    window.display();
  }

  return 0;
}
