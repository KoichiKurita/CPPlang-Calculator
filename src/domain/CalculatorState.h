#pragma once
#include <cstdint>
#include <optional>
#include <string>

// 四則演算の種類。char ではなく列挙型にすることで、不正な演算子を型で排除する
enum class Operator : std::uint8_t { Add, Subtract, Multiply, Divide };

// 保留中の演算（「5 +」まで入力された状態）。左辺と演算子は必ず組で存在する
struct PendingOperation {
    double lhs{0};
    Operator op{Operator::Add};
};

struct CalculatorState {
    // 表示中の値（入力中の数値 or 計算結果）
    double current{0};
    // 保留中の演算（なければ空）
    std::optional<PendingOperation> pending;
    // 入力中の数値の文字列（例: "12.5"、"-0."）。空なら数値を入力していない
    // 小数を文字列で組み立てることで、0.1 を何度も足すような浮動小数点の誤差の蓄積を避ける
    std::string entry;
};
