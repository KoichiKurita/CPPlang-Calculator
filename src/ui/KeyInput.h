#pragma once
#include "ICalculatorUI.h"
#include <optional>

// キーボードで入力された文字を Input に変換する（Qt 非依存の純粋関数としてテスト可能にする）
// Enter / Esc / Backspace のような文字を持たないキーは、Qt 側（QtCalculatorUI）で変換する
inline std::optional<Input> inputFromChar(char c) {
    if (c >= '0' && c <= '9') {
        return Input{InputType::Digit, c - '0'};
    }
    switch (c) {
        case '.': return Input{InputType::DecimalPoint};
        case '+': return Input{InputType::Operator, 0, Operator::Add};
        case '-': return Input{InputType::Operator, 0, Operator::Subtract};
        case '*': return Input{InputType::Operator, 0, Operator::Multiply};
        case '/': return Input{InputType::Operator, 0, Operator::Divide};
        case '=': return Input{InputType::Equal};
        default:  return std::nullopt;
    }
}
