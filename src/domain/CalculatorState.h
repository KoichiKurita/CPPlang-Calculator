#pragma once
#include <optional>

struct CalculatorState {
    double current{0};
    std::optional<double> stored{};
    char op{0};
    bool hasOp{false};

    bool enteringNumber{false}; // ★ 追加
};
