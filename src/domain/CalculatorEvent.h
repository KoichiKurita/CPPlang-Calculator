#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

// エラーの種類。表示文言への変換は UI 層（ErrorMessage.h）で行う
enum class ErrorCode : std::uint8_t {
    NoOperation,    // 演算子なしで = が押された
    DivisionByZero, // 0 で割ろうとした
    Overflow,       // 計算結果が表現できる範囲を超えた
    InvalidDigit,   // 0〜9 以外の数字が入力された
    // 以下は式の入力（Expression.h）のエラー
    InvalidExpression, // 式の書き方が正しくない（位置つき）
    ExpressionTooLong, // 式が長すぎる
    ExpressionTooDeep, // 括弧や単項演算子の入れ子が深すぎる
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
    ErrorCode code{};
    // 式のエラーが見つかった位置（0 始まりの文字位置）。InvalidExpression のときに使う
    std::size_t position{0};
};

using CalculatorEvent = std::variant<DisplayUpdated, Error>;
