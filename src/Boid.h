//
// Created by juaquin on 11/11/24.
//

#ifndef PARALLEL_BOIDS_SRC_BOID_H_
#define PARALLEL_BOIDS_SRC_BOID_H_

#include "Box.h"
#include "Quadtree.h"
#include "linalg.h"
#include "Constants.h"

typedef linalg::aliases::float2 Point;

struct Conditions {
  float visual_radius = 50.0f;
  float protected_radius = 15.0f;

  float avoid_factor = 0.01f;
  float matching_factor = 0.3f;
  float centering_factor = 0.000002f;

  float turn_factor = 0.1f;
};

class Boid {
 private:
  Point position;
  Point velocity;

  bool predator = false;

 public:

  struct Interface {
   public:
    Point idx_position;
    Boid* boid;

    explicit Interface(Boid* b): idx_position(b->position), boid(b) {}

    void update() {
      idx_position = boid->position;
    }

    static quadtree::Box<float> getBox(const Boid::Interface& b) {
      return {b.idx_position.x, b.idx_position.y, 0, 0};
    }

    bool operator==(const Boid::Interface& other) const {
      return idx_position == other.idx_position;
    }
  };

  Boid(const Point &pos, const Point &vel, const bool& predator) : position(pos), velocity(vel), predator(predator) {}

  static Interface getInterface(Boid* p) {
    return Interface{p};
  }

//#pragma omp declare reduction(+ : Point : omp_out = omp_out + omp_in) initializer(omp_priv = {0, 0})
  void updatePosition(const float &timestamp, const quadtree::Quadtree<Boid::Interface, decltype
  (&Boid::Interface::getBox)> &tree, const Conditions& c) {


    Point pos_avg = {0, 0};
    Point vel_avg = {0, 0};
    Point close = {0, 0};
    int neighboring_boids = 0;

    quadtree::Box<float> queryBox(position.x - c.visual_radius, position.y - c.visual_radius, 2 * c.visual_radius, 2 *
    c.visual_radius);
    std::vector<Boid::Interface> neighbors = tree.query(queryBox);

//#pragma omp parallel for reduction(+:neighboring_boids, pos_avg, vel_avg, close)
    for (const auto& neighbor: neighbors) {

      if (neighbor.boid == this) continue;

      const float distance = linalg::length(neighbor.idx_position - this->position);

      if (distance < c.protected_radius) { close += this->position - neighbor.idx_position; }
      else if (distance < c.visual_radius)
      {

        pos_avg += neighbor.idx_position;
        vel_avg += neighbor.boid->velocity;

        neighboring_boids++;
      }

      if (neighbor.boid->predator && distance < c.visual_radius) {
        close += this->position - neighbor.idx_position;
      }

    }

    if (neighboring_boids > 0) {
      pos_avg /= neighboring_boids;
      vel_avg /= neighboring_boids;

      this->velocity += (pos_avg - this->position) * c.centering_factor + (vel_avg - this->velocity) * c
          .matching_factor;
    }

    this->velocity += close * c.avoid_factor;

    this->position += this->velocity * timestamp;

    if (this->position.x - Constants::MARGIN <= 0) {
        this->position.x = Constants::WIDTH - Constants::MARGIN;
    } else if (this->position.x  + Constants::MARGIN >= Constants::WIDTH) {
        this->position.x = Constants::MARGIN;
    }

    if (this->position.y - Constants::MARGIN <= 0) {
        this->position.y = Constants::HEIGHT - Constants::MARGIN;
    } else if (this->position.y + Constants::MARGIN >= Constants::HEIGHT) {
        this->position.y = Constants::MARGIN;
    }
  }

  void draw(SDL_Renderer* renderer) const {
    if (!this->predator)
      SDL_SetRenderDrawColor(renderer, 30, 10, 50, 255);
    else SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
  }
};

#endif //PARALLEL_BOIDS_SRC_BOID_H_