#pragma once

#include <random>

namespace lab1 {
    constexpr auto RANDOM_RANGE = 1e5;
    std::vector<int> getRandomData(std::size_t count) {
        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> uniform_dist(-RANDOM_RANGE, RANDOM_RANGE);
        std::vector<int> data;
        data.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            data.emplace_back(uniform_dist(e1));
        }
        return data;
    }
}