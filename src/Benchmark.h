#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

class Benchmark {
private:
    std::vector<double> times;
    double execution_time = 0.0;
public:
    Benchmark() {
        times.clear();
        execution_time = 0.0;
    };

    void addTime(const double time) { times.push_back(time); }
    void addExecutionTime(const double time) { execution_time += time; }

    [[nodiscard]] double calculateMean() const {
        if (times.empty()) return 0;
        return std::accumulate(times.begin(), times.end(), 0.0) / static_cast<double>(times.size());
    }

    [[nodiscard]] double calculateMedian() const {
        if (times.empty()) return 0;

        std::vector<double> sortedTimes = times;
        std::ranges::sort(sortedTimes);
        const size_t mid = sortedTimes.size() / 2;
        return sortedTimes.size() % 2 == 0
            ? sortedTimes[mid-1] + sortedTimes[mid] / 2.0
            : sortedTimes[mid];
    }

    [[nodiscard]] double calculateStandardDeviation() const {
        if (times.empty()) return 0;

        double mean = calculateMean();
        double sumSquaredDiff = std::transform_reduce( times.begin(), times.end(), 0.0, std::plus<>(), [mean](const double time) { return std::pow(time - mean, 2); } );
        return std::sqrt(sumSquaredDiff / (times.size() - 1));
    }

    void clear() {times.clear();}
    [[nodiscard]] size_t size() const { return  times.size(); }
};

#endif //BENCHMARK_H
