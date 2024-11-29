#include <iostream>
#include <random>

#define PARALLEL

#include "Quadtree.h"
#include "Boid.h"
#include "Helpers.h"

#include "SFML/Graphics.hpp"
using namespace std;

int main(int argc, char *argv[]) {

  if (argc > 1) { Global::NUM_UPDATES = std::stoi(argv[1]); } //* DEFINE UPDATES

  if (argc > 2) { Global::NUM_BOIDS = std::stoi(argv[2]); }   //* DEFINE BOIDS

  if (argc > 3) { Global::NUM_THREADS = std::stoi(argv[3]); } //* DEFINE THREADS


  sf::RenderWindow window(sf::VideoMode(Constants::WIDTH, Constants::HEIGHT), "Boids");
  window.setFramerateLimit(60);

  //* BOIDS GENERATION
  vector<unique_ptr<Boid>> boids;
  vector<Boid::Interface> interfaces;

  std::random_device rd;
  std::mt19937_64 gen(rd());

  std::uniform_real_distribution<float> distributionX(0, Constants::WIDTH);
  std::uniform_real_distribution<float> distributionY(0, Constants::HEIGHT);

  std::uniform_real_distribution<float> angleDistribution(0, 2 * M_PI);
  std::uniform_real_distribution<float> speedDistribution(0.5, 3.0);

  for(int i = 0; i < Global::NUM_BOIDS; i++) {
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

  //* QUADTREE GENERATION
  quadtree::Box<float> universe{0., 0, Constants::WIDTH, Constants::HEIGHT};
  quadtree::Quadtree<Boid::Interface, decltype(&Boid::Interface::getBox)> boids_tree(universe, Boid::Interface::getBox);

  //* QUADTREE: FIRST INSERT BOIDS
#ifndef PARALLEL
  for (const auto& boidref: interfaces) {
    boids_tree.add(boidref);
  }
#else
  boids_tree.bulk_add(interfaces);
#endif

  //* DO SOMETHING?
  sf::Clock deltaclock;

  //* INFINITE EXECUTE
  while (window.isOpen() && Global::NUM_UPDATES > 0) {
    sf::Time elapsed = deltaclock.restart();
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }

    //* BOIDS UPDATE POSITION: IN TIMESTAMP
#ifdef PARALLEL
#pragma omp parallel for default(none) shared(boids, boids_tree) firstprivate(Constants::FIXED_TIME_STEP, Constants::TIME_SCALE) num_threads(Global::NUM_THREADS)
#endif
    for (int i=0; i < boids.size(); ++i) {
      boids[i]->updatePosition(Constants::FIXED_TIME_STEP * Constants::TIME_SCALE, boids_tree);
    }
    std::cerr << Global::NUM_UPDATES << std::endl;

    //* BOIDS::INTERFACE UPDATE
#ifdef PARALLEL
#pragma omp parallel for default(none) shared(interfaces, boids_tree) num_threads(Global::NUM_THREADS)
#endif
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

    //* QUADTREE UPDATE
#ifdef PARALLEL
    boids_tree.clear();
    boids_tree.bulk_add(interfaces);
#endif

    //* REDRAW
    window.clear();
    for (const auto& boid: boids) {
      boid->draw(window);
    }
    window.display();

    Global::NUM_UPDATES--;
  }

  return 0;
}
