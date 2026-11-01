#pragma once

#include "../domain/CalculatorEvent.h"
#include "../domain/CalculatorState.h"
#include <cstdint>
#include <functional>

enum class InputType : std::uint8_t { Digit, Operator, Equal, Clear };

struct Input {
    InputType type{};
    int value{0};
    Operator op{Operator::Add};
};

class ICalculatorUI {
public:
    ICalculatorUI() = default;
    virtual ~ICalculatorUI() = default;

    // インターフェースはコピー・ムーブしない（派生クラスのスライシングを防ぐ）
    ICalculatorUI(const ICalculatorUI&) = delete;
    ICalculatorUI& operator=(const ICalculatorUI&) = delete;
    ICalculatorUI(ICalculatorUI&&) = delete;
    ICalculatorUI& operator=(ICalculatorUI&&) = delete;

    virtual void onInput(std::function<void(Input)> handler) = 0;
    virtual void handleEvent(const CalculatorEvent& event) = 0;
};
