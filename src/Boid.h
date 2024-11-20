//
// Created by juaquin on 11/11/24.
//

#ifndef PARALLEL_BOIDS_SRC_BOID_H_
#define PARALLEL_BOIDS_SRC_BOID_H_

#include "Box.h"
#include "Quadtree.h"
#include "linalg.h"
#include "SFML/Graphics.hpp"
#include "Constants.h"

typedef linalg::aliases::float2 Point;

class Boid {
 private:
  Point position;
  Point velocity;

  bool predator = false;

  float visual_radius = 50.0f;
  float protected_radius = 15.0f;

  float avoid_factor = 0.01f;
  float matching_factor = 0.3f;
  float centering_factor = 0.000002f;

  float turn_factor = 0.1f;

 public:

  struct Interface {
    Point idx_position;
    std::shared_ptr<Boid> boid;

    explicit Interface(const std::shared_ptr<Boid>& b): idx_position(b->position), boid(b) {}

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

  static Interface getInterface(const std::shared_ptr<Boid>& p) {
    return Interface{p};
  }

  void updatePosition(const float &timestamp, const quadtree::Quadtree<Boid::Interface, decltype(&Boid::Interface::getBox)> &tree) {

    float xpos_avg = 0.0f, ypos_avg = 0.0f;
    float xvel_avg = 0.0f, yvel_avg = 0.0f;
    float close_dx = 0.0f, close_dy = 0.0f;
    float neighboring_boids = 0;

    quadtree::Box<float> queryBox(position.x - visual_radius, position.y - visual_radius, 2 * visual_radius, 2 * visual_radius);
    std::vector<Boid::Interface> neighbors = tree.query(queryBox);

    for (const auto& neighbor: neighbors) {

      if (neighbor.boid.get() == this) continue;

      float dx = neighbor.idx_position.x - this->position.x;
      float dy = neighbor.idx_position.y - this->position.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance < protected_radius)
      {
        close_dx += this->position.x - neighbor.idx_position.x;
        close_dy += this->position.y - neighbor.idx_position.y;
      }
      else if (distance < visual_radius)
      {
        xpos_avg += neighbor.idx_position.x;
        ypos_avg += neighbor.idx_position.y;

        xvel_avg += neighbor.boid->velocity.x;
        yvel_avg += neighbor.boid->velocity.y;

        neighboring_boids++;
      }

      if (neighbor.boid->predator && distance < visual_radius) {
        close_dx += this->position.x - neighbor.idx_position.x;
        close_dy += this->position.y - neighbor.idx_position.y;
      }

    }

    if (neighboring_boids > 0) {
      xpos_avg /= neighboring_boids;
      ypos_avg /= neighboring_boids;

      xvel_avg /= neighboring_boids;
      yvel_avg /= neighboring_boids;

      this->velocity.x += (xpos_avg - this->position.x) * centering_factor + (xvel_avg - this->velocity.x) * matching_factor;
      this->velocity.y += (ypos_avg - this->position.y) * centering_factor + (yvel_avg - this->velocity.y) * matching_factor;
    }

    this->velocity.x += close_dx * avoid_factor;
    this->velocity.y += close_dy * avoid_factor;

    this->position.x += this->velocity.x * timestamp;
    this->position.y += this->velocity.y * timestamp;

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

  void draw(sf::RenderWindow& window) const {
    sf::CircleShape circle(5.);
    circle.setPosition(position.x, position.y);
    circle.setFillColor(predator ? sf::Color::Red : sf::Color::Green);
    window.draw(circle);
  }
};

#endif //PARALLEL_BOIDS_SRC_BOID_H_