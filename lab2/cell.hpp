#pragma once

namespace lab2 {
    class Cell {
    public:
        enum class State { Dead, Alive, None };
        State state {State::Dead};

        explicit Cell(State state) : state {state} {}
        Cell() = default;
    };
}