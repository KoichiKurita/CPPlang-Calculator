#pragma once

#include "../domain/CalculatorEvent.h"
#include <functional>

enum class InputType {
    Digit,
    Operator,
    Equal,
    Clear
};

struct Input {
    InputType type;
    int value{0};
    char op{0};
};

class ICalculatorUI {
public:
    virtual ~ICalculatorUI() = default;

    virtual void onInput(std::function<void(Input)> handler) = 0;
    virtual void handleEvent(const CalculatorEvent& event) = 0;
};
