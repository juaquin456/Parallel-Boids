//
// Created by aaroncs25 on 11/19/24.
//

#ifndef HELPERS_H
#define HELPERS_H

namespace Global {
    inline int NUM_UPDATES = 100;
    inline int NUM_BOIDS = 1000;
    inline int NUM_THREADS = 4;
}

namespace Constants {
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    constexpr int MARGIN = 40;
    constexpr float FIXED_TIME_STEP = 16.67f;
    constexpr float TIME_SCALE = 0.2f;
}

#endif //HELPERS_H
