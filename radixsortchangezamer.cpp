#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <ctime>

long long char_comparisons = 0;
const int Threshold = 74;

int get_char(const std::string &s, int index) {
    char_comparisons++;
    if (index < s.size()) {
        return static_cast<unsigned char>(s[index]);
    }
    return -1;
}

void ternary_quick_sort(std::vector<std::string> &arr, int low, int high, int d) {
    if (low >= high) return;

    int pivot = get_char(arr[low], d);
    int lt = low, gt = high, i = low + 1;
    while (i <= gt) {
        int t = get_char(arr[i], d);
        if (t < pivot) {
            std::swap(arr[lt++], arr[i++]);
        } else if (t > pivot) {
            std::swap(arr[i], arr[gt--]);
        } else {
            i++;
        }
    }

    ternary_quick_sort(arr, low, lt - 1, d);
    if (pivot != -1) ternary_quick_sort(arr, lt, gt, d + 1);
    ternary_quick_sort(arr, gt + 1, high, d);
}

void msd_radix_sort(std::vector<std::string> &arr, int low, int high, int d) {
    if (low >= high) return;

    if (high - low + 1 < Threshold) {
        ternary_quick_sort(arr, low, high, d);
        return;
    }

    const int kNumBuckets = 258;
    std::vector<int> count(kNumBuckets, 0);

    for (int i = low; i <= high; i++) {
        int c = get_char(arr[i], d);
        count[c + 2]++;
    }

    std::vector<int> prefix(kNumBuckets + 1, 0);
    for (int i = 1; i <= kNumBuckets; i++) {
        prefix[i] = prefix[i - 1] + count[i - 1];
    }

    std::vector<std::string> temp(high - low + 1);
    for (int i = low; i <= high; i++) {
        int c = get_char(arr[i], d);
        temp[prefix[c + 2]++] = arr[i];
    }

    for (int i = 0; i < temp.size(); i++) {
        arr[low + i] = temp[i];
    }

    for (int b = 0; b < kNumBuckets - 1; b++) {
        int start = low + prefix[b];
        int end = low + prefix[b + 1] - 1;
        if (start < end) {
            msd_radix_sort(arr, start, end, d + 1);
        }
    }
}

class StringGenerator {
private:
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-.";
    std::mt19937 rng;
    std::uniform_int_distribution<int> lengthDist;
    std::uniform_int_distribution<int> charDist;

    std::string randomString(int length) {
        std::string result;
        for (int i = 0; i < length; i++) {
            result += charset[charDist(rng)];
        }
        return result;
    }

public:
    StringGenerator(int minLen = 10, int maxLen = 200)
        : rng(static_cast<unsigned int>(std::time(nullptr))),
          lengthDist(minLen, maxLen),
          charDist(0, static_cast<int>(charset.size()) - 1) {}

    std::vector<std::string> generateRandomArray(int size) {
        std::vector<std::string> result;
        for (int i = 0; i < size; i++) {
            result.push_back(randomString(lengthDist(rng)));
        }
        return result;
    }

    std::vector<std::string> generateReverseSortedArray(int size) {
        auto arr = generateRandomArray(size);
        std::sort(arr.begin(), arr.end(), std::greater<std::string>());
        return arr;
    }

    std::vector<std::string> generateAlmostSortedArray(int size, int swaps = 20) {
        auto arr = generateRandomArray(size);
        std::sort(arr.begin(), arr.end());
        for (int i = 0; i < swaps; i++) {
            int idx1 = rng() % size;
            int idx2 = rng() % size;
            std::swap(arr[idx1], arr[idx2]);
        }
        return arr;
    }
};

std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", std::localtime(&in_time_t));
    return std::string(buf);
}

void run_experiments(const std::string &array_type) {
    StringGenerator gen;
    std::string filename = "results_msd_radix_fallback_" + array_type + "_" + get_timestamp() + ".csv";
    std::ofstream fout(filename);
    fout << "Size,Time_ms,CharComparisons\n";

    for (int size = 100; size <= 3000; size += 100) {
        std::vector<std::string> arr;
        if (array_type == "random") {
            arr = gen.generateRandomArray(size);
        } else if (array_type == "reverse") {
            arr = gen.generateReverseSortedArray(size);
        } else if (array_type == "almost") {
            arr = gen.generateAlmostSortedArray(size);
        } else {
            std::cerr << "Invalid array type.\n";
            return;
        }

        char_comparisons = 0;
        auto start = std::chrono::high_resolution_clock::now();
        msd_radix_sort(arr, 0, (int)arr.size() - 1, 0);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        fout << size << "," << duration.count() << "," << char_comparisons << "\n";
        std::cout << "Done: " << size << " (" << array_type << ")\n";
    }

    fout.close();
    std::cout << "Results saved to " << filename << "\n";
}

int main() {
    std::string type;
    std::cout << "Enter array type [random | reverse | almost]: ";
    std::cin >> type;

    run_experiments(type);

    return 0;
}
