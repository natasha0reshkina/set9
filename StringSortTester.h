#ifndef STRING_SORT_TESTER_H
#define STRING_SORT_TESTER_H

#include "StringGenerator.h"
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <chrono>

class StringSortTester {
public:
    using SortFunction = std::function<void(std::vector<std::string>&, long long&)>;

    StringSortTester(SortFunction sort_func) : sort_func(sort_func) {}

    void run_experiments(const std::string& mode, int min_size, int max_size, int step, const std::string& filename) {
        StringGenerator generator;
        std::ofstream file(filename);
        file << "Size,Time_ms,CharComparisons\n";

        for (int size = min_size; size <= max_size; size += step) {
            std::vector<std::string> data;

            if (mode == "random") {
                data = generator.generate_random(size);
            } else if (mode == "almost") {
                int swaps = std::max(1, size / 20); // 5% перестановок
                data = generator.generate_almost_sorted(size, swaps);
            } else if (mode == "reverse") {
                data = generator.generate_reverse(size);
            } else {
                continue;
            }

            long long comparisons = 0;
            auto start = std::chrono::high_resolution_clock::now();
            sort_func(data, comparisons);
            auto end = std::chrono::high_resolution_clock::now();
            long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            file << size << "," << duration << "," << comparisons << "\n";
        }
    }

private:
    SortFunction sort_func;
};

#endif
