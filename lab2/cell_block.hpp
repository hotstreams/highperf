#pragma once

#include <vector>
#include <filesystem>

#include "cell.hpp"

namespace lab2 {
    using vec2 = std::pair<int, int>;
    class Block {
    private:
        std::vector<std::vector<Cell>> cells;
        vec2 block_index;
        std::size_t size;
        std::size_t block_count;
    public:
        Block(std::size_t size, std::size_t block_count, vec2 index);
        ~Block() = default;

        void setState(const std::vector<std::vector<int>>& state);
        void saveState(const std::filesystem::path& path, int iter);
        std::size_t getAliveNeighbours(vec2 index);

        void update();
        void send();
        void recv();
    };

    std::vector<int> getAdjacentRanks(size_t block_size, vec2 index);
}
