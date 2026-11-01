#pragma once
#include <cstdint>
#include <string>
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
    // 入力中の文字列。空でなければ UI はこれをそのまま表示する
    // （"0." や "1.50" のような入力途中の表示を保つため）
    //
    // {} は、DisplayUpdated{value} と entry を省略して初期化したときの
    // GCC の -Wmissing-field-initializers を防ぐために付けている。
    // clang-tidy の readability-redundant-member-init とは両立しないため抑制する。
    // NOLINTNEXTLINE(readability-redundant-member-init)
    std::string entry{};
};

struct Error {
    ErrorCode code;
};

using CalculatorEvent = std::variant<DisplayUpdated, Error>;
