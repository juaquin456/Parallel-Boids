//
// Created by juaquin on 11/11/24.
//

#ifndef PARALLEL_BOIDS_SRC_BOID_H_
#define PARALLEL_BOIDS_SRC_BOID_H_

#include "Box.h"
#include "Quadtree.h"
#include "linalg.h"


typedef linalg::aliases::float2 Point;

class Boid {
 private:
  Point position;
  Point velocity;
 public:
  Boid(const Point &pos, const Point &vel) : position(pos), velocity(vel) {}

  static quadtree::Box<float> getBox(const std::shared_ptr<Boid>& b) {
    return {b->position.x, b->position.y, 0, 0};
  }
  void updatePosition(const float &timestamp, const quadtree::Quadtree<std::shared_ptr<Boid>, decltype(&Boid::getBox)
  > &tree) {
    // complete your code camachito
  }
};

#endif //PARALLEL_BOIDS_SRC_BOID_H_
