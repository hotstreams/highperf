#pragma once

namespace lab1 {
    class Measurer {
    private:
        std::string name;
        double start;
        double end {};
    public:
        Measurer(std::string name)
            : name {std::move(name)}
            , start {omp_get_wtime()} {
        }

        ~Measurer() {
            end = omp_get_wtime();
            std::cout << "Measured time " << name << " " << end - start << std::endl;
        }
    };
}