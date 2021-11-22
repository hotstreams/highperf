#include "cell_block.hpp"

#include <mpi.h>
#include <fstream>

using namespace lab2;

Block::Block(size_t size, size_t block_count, vec2 index)
    : block_index(index)
    , size { size }
    , block_count {block_count} {
    for (int i = 0; i < size + size * 2; ++i) {
        cells.emplace_back();
        for (int j = 0; j < size + size * 2; ++j) {
            if (i >= size && i < size + size && j >= size && j < size + size) {
                cells[i].emplace_back();
            } else {
                cells[i].emplace_back(Cell::State::None);
            }
        }
    }
}

size_t Block::getAliveNeighbours(vec2 index) {
    size_t alive {};
    for (int i = index.first - 1; i <= index.first + 1; ++i) {
        for (int j = index.second - 1; j <= index.second + 1; ++j) {
            if (i == index.first && j == index.second) {
                continue;
            }

            if (cells.at(i).at(j).state == Cell::State::Alive) {
                ++alive;
            }
        }
    }
    return alive;
}

void Block::send() {
    int current {};
    MPI_Comm_rank(MPI_COMM_WORLD, &current);

    std::vector<int> data;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            data.emplace_back(static_cast<int>(cells.at(i + size).at(j + size).state));

    const auto ranks = getAdjacentRanks(block_count, block_index);
    for (const auto &rank : ranks) {
        MPI_Send(data.data(), sizeof(int) * data.size(), MPI_BYTE, rank, 1, MPI_COMM_WORLD);
        std::cout << "Rank " << current << " sending to " << rank << std::endl;
    }
}

void Block::recv() {
    auto data = std::vector(size * size, 0);

    const auto ranks = getAdjacentRanks(block_count, block_index);
    for (int i = 0; i < ranks.size(); ++i) {
        MPI_Status status;
        MPI_Recv(data.data(), sizeof(int) * data.size(), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        vec2 source_index = {status.MPI_SOURCE / block_count, status.MPI_SOURCE % block_count};
        vec2 diff = {block_index.first - 1, block_index.second - 1};
        vec2 res = {source_index.first - diff.first, source_index.second - diff.second};

        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                cells.at(res.first * size + j).at(res.second * size + k).state = static_cast<Cell::State>(data.at(j * size + k));
            }
        }
    }
}

void Block::update() {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            const auto alive_n = getAliveNeighbours({i + size, j + size});
            auto& cell = cells.at(i + size).at(j + size);
            if (cell.state == Cell::State::None) { continue; }
            if (cell.state == Cell::State::Alive) {
                if (alive_n < 2) {
                    cell.state = Cell::State::Dead;
                }

                if (alive_n == 2 || alive_n == 3) {
                    cell.state = Cell::State::Alive;
                }

                if (alive_n > 3) {
                    cell.state = Cell::State::Dead;
                }
            } else {
                if (alive_n == 3) {
                    cell.state = Cell::State::Alive;
                }
            }
        }
    }
}

void Block::setState(const std::vector<std::vector<int>>& state) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            cells.at(i + size).at(j + size).state = static_cast<Cell::State>(state.at(block_index.first * size + i).at(block_index.second * size + j));
        }
    }
}

void Block::saveState(const std::filesystem::path& path, int iter) {
    int current {};
    MPI_Comm_rank(MPI_COMM_WORLD, &current);

    std::ofstream file {path.string() + "/block_" + std::to_string(current) + "_iter_" + std::to_string(iter)};
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            file << static_cast<int>(cells.at(i + size).at(j + size).state) << " ";
        }
        file << "\n";
    }
}

std::vector<int> lab2::getAdjacentRanks(size_t block_size, vec2 index) {
    std::vector<std::vector<int>> map;
    int k {};
    for (int i = 0; i < block_size; ++i) {
        map.emplace_back();
        for (int j = 0; j < block_size; ++j) {
            map[i].emplace_back(k++);
        }
    }

    const auto& [x, y] = index;

    std::vector<int> ranks;
    for (int i = x  - 1; i <= x + 1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            if (i == x && j == y) {
                continue;
            }

            if (i < 0 || j < 0 || i > block_size - 1 || j > block_size - 1) {
                continue;
            }

            ranks.emplace_back(map.at(i).at(j));
        }
    }
    return ranks;
}
