#pragma once
#include <variant>

struct DisplayUpdated {
    double value;
};

struct Error {
    const char* message;
};

using CalculatorEvent = std::variant<DisplayUpdated, Error>;
