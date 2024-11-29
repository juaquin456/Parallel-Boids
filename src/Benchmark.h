#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <algorithm>
#include <numeric>
#include <vector>

class Benchmark {
private:
    std::vector<double> times;
public:
    void addTime(const double time) { times.push_back(time); }

    double calculateMean() {
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



};

#endif //BENCHMARK_H
