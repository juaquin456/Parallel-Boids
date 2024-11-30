#include <iostream>
#include <random>

#define PARALLEL

#include "Benchmark.h"
#include "Quadtree.h"
#include "Boid.h"
#include "Helpers.h"

using namespace std;

void sequential_boids(Benchmark& benchmark) {
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
  for (const auto& boidref: interfaces) {
    boids_tree.add(boidref);
  }

  //* INFINITE EXECUTE
  while (Global::NUM_UPDATES > 0) {
    auto start_time = chrono::high_resolution_clock::now();

    //* BOIDS UPDATE POSITION: IN TIMESTAMP
    for (const auto & boid : boids) {
      boid->updatePosition(Constants::FIXED_TIME_STEP * Constants::TIME_SCALE, boids_tree);
    }

    //* BOIDS::INTERFACE UPDATE
    for (auto & boid : interfaces) {
      boids_tree.remove(boid);
      boid.update();
      boids_tree.add(boid);
    }

    //* QUADTREE UPDATE
    auto end_time = chrono::high_resolution_clock::now();
    const double elapsed = chrono::duration<double, milli>(end_time - start_time).count();
    benchmark.addTime(elapsed);

    Global::NUM_UPDATES--;
  }
}

void parallel_boids(Benchmark& benchmark) {
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
  boids_tree.bulk_add(interfaces);

  //* INFINITE EXECUTE
  while (Global::NUM_UPDATES > 0) {
    auto start_time = chrono::high_resolution_clock::now();

    //* BOIDS UPDATE POSITION: IN TIMESTAMP
#pragma omp parallel for num_threads(Global::NUM_THREADS)
    for (int i=0; i < boids.size(); ++i) {
      boids[i]->updatePosition(Constants::FIXED_TIME_STEP * Constants::TIME_SCALE, boids_tree);
    }
#pragma omp parallel sections num_threads(Global::NUM_THREADS)
{
#pragma omp section
      {
#pragma omp parallel for
        for (int i = 0; i < interfaces.size(); i++) {
          interfaces[i].update();
        }
      }
#pragma omp section
      {
        boids_tree.clear();
      }

}

    //* BOIDS::INTERFACE UPDATE



    //* QUADTREE UPDATE
    boids_tree.bulk_add(interfaces);

    auto end_time = chrono::high_resolution_clock::now();
    const double elapsed = chrono::duration<double, milli>(end_time - start_time).count();
    benchmark.addTime(elapsed);

    Global::NUM_UPDATES--;
  }
}

int main(const int argc, char *argv[]) {

  bool isSequential = false;

  if (argc > 1) { isSequential = std::stoi(argv[1]); }

  if (argc > 2) { Global::NUM_BOIDS = std::stoi(argv[2]); }   //* DEFINE BOIDS

  if (argc > 3) { Global::NUM_THREADS = std::stoi(argv[3]); } //* DEFINE THREADS

  auto benchmark = Benchmark();

  const auto start_time = chrono::high_resolution_clock::now();

  if (isSequential) {
    sequential_boids(benchmark);
  }
  else {
    parallel_boids(benchmark);
  }

  const auto end_time = chrono::high_resolution_clock::now();
  const double total_duration = chrono::duration<double, milli>(end_time - start_time).count();

  cout << "Resultados para " << Global::NUM_THREADS << " threads y " << Global::NUM_BOIDS << " boids:" << endl;
  cout << "  Tiempo promedio (ms): " << benchmark.calculateMean() << endl;
  cout << "  Tiempo total de ejecución (ms): " << total_duration << endl;
  cout << endl;
  benchmark.clear();

  return 0;
}
