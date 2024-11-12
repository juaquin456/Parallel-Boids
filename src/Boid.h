//
// Created by juaquin on 11/11/24.
//

#ifndef PARALLEL_BOIDS_SRC_BOID_H_
#define PARALLEL_BOIDS_SRC_BOID_H_

#include "Box.h"
#include "Quadtree.h"
#include "linalg.h"
#include "SFML/Graphics.hpp"

typedef linalg::aliases::float2 Point;

class Boid {
 private:
  Point position;
  Point velocity;
 public:
  struct Interface {
    Point idx_position;
    std::shared_ptr<Boid> boid;
    explicit Interface(const std::shared_ptr<Boid>& b): boid(b), idx_position(b->position) {}
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

  Boid(const Point &pos, const Point &vel) : position(pos), velocity(vel) {}

  static Interface getInterface(const std::shared_ptr<Boid>& p) {
    return Interface{p};
  }
  void updatePosition(const float &timestamp, const quadtree::Quadtree<Boid::Interface, decltype(&Boid::Interface::getBox)> &tree) {
    // TODO: update directly this->position
  }
  void draw(sf::RenderWindow& window) const {
    sf::CircleShape circle(5.);
    circle.setPosition(position.x, position.y);
    circle.setFillColor(sf::Color::Green);
    window.draw(circle);
  }
};

#endif //PARALLEL_BOIDS_SRC_BOID_H_
