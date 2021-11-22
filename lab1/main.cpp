#include <iostream>
#include <omp.h>

#include "insertion_sort.hpp"
#include "tree.hpp"
#include "random.hpp"
#include "measurer.hpp"

auto getThreadsNum() {
    std::cout << "Enter threads num: " << std::endl;
    int32_t num {};
    std::cin >> num;
    if (num <= 0) {
        throw std::logic_error {"Num should be >= 0"};
    }
    return num;
}

auto getContainerCount() {
    int32_t count {};
    std::cout << "Enter size of sorted elements: " << std::endl;
    std::cin >> count;
    if (count <= 0) {
        throw std::logic_error {"Size should be >= 0"};
    }
    return count;
}

auto getData() {
    return lab1::getRandomData(getContainerCount());
}

auto getAlgo() {
    std::cout << "Choose algo:\n1. Insertion sort\n2. Insertion sort using OMP\n3. Tree sort\n4. Tree sort using OMP\n5. Quit\n" << std::endl;
    uint32_t algo {};
    std::cin >> algo;
    return algo;
}

int main(int argc, char** argv) {
    using namespace lab1;

    bool stop {};

    while (!stop) {
        switch (getAlgo()) {
            case 1: {
                auto data = getData();
                {
                    Measurer time {"Total"};
                    insertion_sort(data, 0, data.size());
                }
                break;
            }
            case 3: {
                auto data = getData();
                {
                    Measurer time {"Total"};
                    tree_sort(data, 0, data.size());
                }
                break;
            }
            case 2: {
                auto data = getData();
                omp_set_num_threads(getThreadsNum());
                {
                    Measurer time {"Total"};
                    insertion_sort_omp(data);
                }
                break;
            }
            case 4: {
                auto data = getData();
                omp_set_num_threads(getThreadsNum());
                {
                    Measurer time {"Total"};
                    tree_sort_omp(data);
                }
                break;
            }
            case 5:
                stop = true;
                break;
            default:
                std::cout << "Wrong entered algo" << std::endl;
                break;
        }
    }

    return 0;
}
