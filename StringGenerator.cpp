#include "StringGenerator.h"
#include <algorithm>
#include <random>
#include <ctime>

static const std::string CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-.";
static std::mt19937 rng((unsigned)time(nullptr));

StringGenerator::StringGenerator() {
    master_array = generate_random(3000);
}

std::string StringGenerator::random_string() {
    std::uniform_int_distribution<> len_dist(10, 200);
    std::uniform_int_distribution<> char_dist(0, CHARSET.size() - 1);
    int len = len_dist(rng);
    std::string s(len, ' ');
    for (int i = 0; i < len; ++i)
        s[i] = CHARSET[char_dist(rng)];
    return s;
}

std::vector<std::string> StringGenerator::generate_random(int size) {
    return std::vector<std::string>(master_array.begin(), master_array.begin() + size);
}

std::vector<std::string> StringGenerator::generate_reverse(int size) {
    auto arr = generate_random(size);
    std::sort(arr.begin(), arr.end(), std::greater<>());
    return arr;
}

std::vector<std::string> StringGenerator::generate_almost_sorted(int size, int swaps) {
    auto arr = generate_random(size);
    std::sort(arr.begin(), arr.end());
    std::uniform_int_distribution<> dist(0, size - 1);
    for (int i = 0; i < swaps; ++i) {
        int x = dist(rng), y = dist(rng);
        std::swap(arr[x], arr[y]);
    }
    return arr;
}

std::vector<std::string> StringGenerator::generate_with_prefix(int size, const std::string& prefix) {
    auto arr = generate_random(size);
    for (auto& str : arr) {
        str = prefix + str;
    }
    return arr;
}
