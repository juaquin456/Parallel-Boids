#include <iostream>
#include <random>
#include <omp.h>

#define PARALLEL

#include "Quadtree.h"
#include "Boid.h"
#include "Constants.h"

#include "SFML/Graphics.hpp"
using namespace std;

int main() {
  sf::RenderWindow window(sf::VideoMode(Constants::WIDTH, Constants::HEIGHT), "Boids");
  window.setFramerateLimit(60);

  vector<unique_ptr<Boid>> boids;
  vector<Boid::Interface> interfaces;

  std::random_device rd;
  std::mt19937_64 gen(rd());

  std::uniform_real_distribution<float> distributionX(0, Constants::WIDTH);
  std::uniform_real_distribution<float> distributionY(0, Constants::HEIGHT);

  std::uniform_real_distribution<float> angleDistribution(0, 2 * M_PI);
  std::uniform_real_distribution<float> speedDistribution(0.5, 3.0);

  for(int i = 0; i < 1000; i++) {
    float x = distributionX(gen);
    float y = distributionY(gen);

    float angle = angleDistribution(gen);
    float speed = speedDistribution(gen);

    float vx = speed * cos(angle);
    float vy = speed * sin(angle);

    std::uniform_int_distribution<int> predator_chance(0, 50);
    bool isPredator = (predator_chance(gen) == 0);

    boids.emplace_back(make_unique<Boid>(Point{x, y}, Point{vx, vy}, isPredator));
    interfaces.push_back(Boid::getInterface(boids[i].get()));
  }

  quadtree::Box<float> universe{0., 0, Constants::WIDTH, Constants::HEIGHT};
  quadtree::Quadtree<Boid::Interface, decltype(&Boid::Interface::getBox)> boids_tree(universe, Boid::Interface::getBox);

#ifndef PARALLEL
  for (const auto& boidref: interfaces) {
    boids_tree.add(boidref);
  }
#else
  boids_tree.bulk_add(interfaces);
#endif

  sf::Clock deltaclock;
  while (window.isOpen()) {
    sf::Time elapsed = deltaclock.restart();
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }

#pragma omp parallel for
    for (int i = 0; i < boids.size(); i++) {
      boids[i]->updatePosition(Constants::FIXED_TIME_STEP * Constants::TIME_SCALE, boids_tree);
    }
    std::cerr << elapsed.asSeconds() << std::endl;

#pragma omp parallel for
    for (int i = 0; i < interfaces.size(); i++) {
#ifndef PARALLEL
      Boid::Interface& boid = interfaces[i];
      boids_tree.remove(boid);
      boid.update();
      boids_tree.add(boid);
#else
      interfaces[i].update();
#endif
    }

#ifdef PARALLEL
    boids_tree.clear();
    boids_tree.bulk_add(interfaces);
#endif
    // redraw
    window.clear();
    for (const auto& boid: boids) {
      boid->draw(window);
    }
    window.display();
  }
  return 0;
}
