#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

class Benchmark {
private:
    double time_sum;
    int update_num;

public:
    Benchmark() {
        time_sum = 0.0;
        update_num = 0;
    };

    void addTime(const double time) { time_sum += time; update_num++;}

    [[nodiscard]] double calculateMean() const {
        if (update_num <= 0) { return 0.0; }
        return time_sum / update_num;
    }

    void clear() {time_sum = 0.0; update_num = 0; };
};

#endif //BENCHMARK_H
