#include <iostream>
#include <random>


#include "Quadtree.h"
#include "Boid.h"
#include "Constants.h"

#include "SFML/Graphics.hpp"

using namespace std;

int main() {
  sf::RenderWindow window(sf::VideoMode(Constants::WIDTH, Constants::HEIGHT), "Boids");
  vector<shared_ptr<Boid>> boids;
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

    boids.emplace_back(make_shared<Boid>(Point{x, y}, Point{vx, vy}, isPredator));
    interfaces.push_back(Boid::getInterface(boids[i]));
  }

  quadtree::Box<float> universe{0., 0, Constants::WIDTH, Constants::HEIGHT};
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

    //#pragma omp parallel for default(none) shared(boids, Constants::FIXED_TIME_STEP, Constants::TIME_SCALE, boids_tree) num_threads(Constants::NUM_THREADS)
    for (auto &boid : boids) {
      //boid->updatePosition(static_cast<float>(elapsed.asMilliseconds()), boids_tree);
      boid->updatePosition(Constants::FIXED_TIME_STEP * Constants::TIME_SCALE, boids_tree);
    }

    std::cout << elapsed.asSeconds() << std::endl;

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
