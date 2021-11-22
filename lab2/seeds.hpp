#pragma once

#include <mpi.h>
#include <fstream>
#include "cell_block.hpp"
#include <cmath>

namespace lab2 {
    class Seeds {
    private:
        static void
        mergeStates(const std::filesystem::path &path, size_t thread_count, size_t iter_count, size_t block_count, size_t block_size) {
            for (int i = 0; i < iter_count; ++i) {
                std::vector<std::vector<int>> state;
                state.resize(block_count * block_size);
                for (auto &v: state) {
                    v.resize(block_count * block_size);
                }

                int c = 0;
                for (int a = 0; a < block_count; ++a) {
                    for (int b = 0; b < block_count; ++b) {
                        std::ifstream file{
                                path.string() + "/block_" + std::to_string(c++) + "_iter_" + std::to_string(i)};
                        file.exceptions(std::ios::badbit);

                        for (int k = 0; k < block_size; ++k) {
                            for (int l = 0; l < block_size; ++l) {
                                int cell{};
                                file >> cell;
                                state.at(a * block_size + k).at(b * block_size + l) = cell;
                            }
                        }
                    }
                }

                std::ofstream ofile{path.string() + "/full_state_iter_" + std::to_string(i)};
                for (const auto &item: state) {
                    for (const auto &item1: item) {
                        ofile << item1 << ' ';
                    }
                    ofile << '\n';
                }
            }
        }

    public:
        static void run(int argc, char **argv) {
            if (argc != 4) {
                throw std::logic_error("lab2 [path_to_state] [path_to_save] [simulation_count]");
            }

            MPI_Init(&argc, &argv);

            int rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);

            int count;
            MPI_Comm_size(MPI_COMM_WORLD, &count);

            std::ifstream file;
            file.exceptions(std::ios::badbit | std::ios::failbit);
            try {
                file.open(argv[1]);
            } catch (...) {
                throw std::runtime_error{"Cannot open file with state!"};
            }

            file.exceptions(std::ios::badbit);

            std::vector<int> state;
            int cell{};
            while (file >> cell) {
                state.emplace_back(cell);
            }

            std::vector<std::vector<int>> map;

            int iter{};
            for (int i = 0; i < sqrt(state.size()); ++i) {
                map.emplace_back();
                for (int j = 0; j < sqrt(state.size()); ++j) {
                    map[i].emplace_back(state[iter++]);
                }
            }

            size_t size = sqrt(state.size());
            size_t block_count = std::sqrt(count);

            vec2 index = {rank / block_count, rank % block_count};

            std::cout << "RANK = " << rank << " " << index.first << " " << index.second << std::endl;

            Block block{size / block_count, block_count, index};

            block.setState(map);
            block.saveState(argv[2], 0);

            for (int i = 1; i <= std::stoi(argv[3]); ++i) {
                if (count != 1) {
                    block.send();
                    block.recv();
                }

                block.update();

                block.saveState(argv[2], i);
            }

            mergeStates(argv[2], count, std::stoi(argv[3]), block_count, size / block_count);

            MPI_Finalize();
        };
    };
}