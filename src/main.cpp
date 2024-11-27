#include <iostream>
#include <random>
#include <omp.h>
#include "GUI.h"
#include "Quadtree.h"
#include "Boid.h"
#include "Constants.h"

using namespace std;

pair<vector<unique_ptr<Boid>>, vector<Boid::Interface>> generate_boids() {
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
  return {std::move(boids), std::move(interfaces)};
}

int main() {
  GUI app{};
  if (app.init() == -1) return -1;

  quadtree::Box<float> universe{0., 0, Constants::WIDTH, Constants::HEIGHT};
  quadtree::Quadtree<Boid::Interface, decltype(&Boid::Interface::getBox)> boids_tree(universe, Boid::Interface::getBox);
  auto [boids, interfaces] = generate_boids();
  boids_tree.bulk_add(interfaces);

  while (app.is_running()) {
    app.process_event();
#pragma omp parallel for default(none) shared(boids, boids_tree) num_threads(Constants::NUM_THREADS)
    for (int i=0; i < boids.size(); ++i) {
      boids[i]->updatePosition(Constants::FIXED_TIME_STEP * Constants::TIME_SCALE, boids_tree);
    }

#pragma omp parallel for
    for (int i = 0; i < interfaces.size(); i++) {
      interfaces[i].update();
    }
    boids_tree.clear();
    boids_tree.bulk_add(interfaces);

    SDL_SetRenderDrawColor(app.get_renderer(), 120, 180, 255, 255);
    SDL_RenderClear(app.get_renderer());
    for(auto& boid: boids) boid->draw(app.get_renderer());
    app.draw();
  }
  app.destroy();
  return 0;
}
