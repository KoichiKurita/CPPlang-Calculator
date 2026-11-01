#pragma once
#include <cstdint>
#include <variant>

// エラーの種類。表示文言への変換は UI 層（ErrorMessage.h）で行う
enum class ErrorCode : std::uint8_t {
    NoOperation,    // 演算子なしで = が押された
    DivisionByZero, // 0 で割ろうとした
    Overflow,       // 計算結果が表現できる範囲を超えた
    InvalidDigit,   // 0〜9 以外の数字が入力された
};

struct DisplayUpdated {
    double value;
};

struct Error {
    ErrorCode code;
};

using CalculatorEvent = std::variant<DisplayUpdated, Error>;
