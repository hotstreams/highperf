#pragma once

#include "measurer.hpp"

namespace lab1 {
    template <typename Value, template <typename> class SequenceContainer>
    void insertion_sort(SequenceContainer<Value>& container, std::size_t start, std::size_t end) {
        for (std::size_t i = start; i < end; ++i) {
            Value value = container[i];
            int32_t j = i - 1;
            while (j >= 0 && container[j] > value) {
                container[j + 1] = container[j];
                --j;
            }
            container[j + 1] = value;
        }
    }

    template <typename Value, template <typename> class SequenceContainer>
    void insertion_sort(SequenceContainer<Value>& container) {
        insertion_sort(container, 0, container.size());
    }

    template <typename Value, template <typename> class SequenceContainer>
    void insertion_sort_omp(SequenceContainer<Value>& container) {
        {
            lab1::Measurer time {"insertion sort split"};

            #pragma omp parallel
            {
                auto count = omp_get_num_threads();
                auto current = omp_get_thread_num();
                insertion_sort(container, container.size() * current / count, container.size() * (current + 1) / count);
            }
        }

        {
            lab1::Measurer time {"insertion sort merge"};
            insertion_sort(container, 0, container.size());
        }
    }
}