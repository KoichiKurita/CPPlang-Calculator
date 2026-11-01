#pragma once
#include "CalculatorEvent.h"
#include "CalculatorState.h"
#include <cmath>
#include <utility>

// 入力できる数値の上限（15桁）。double で誤差なく扱える整数の範囲内に収める
inline constexpr double kMaxInputValue = 1e15;

// ① calculate
inline std::pair<CalculatorState, CalculatorEvent> calculate(const CalculatorState& s) {
    if (!s.pending) {
        return {s, Error{ErrorCode::NoOperation}};
    }

    const auto [lhs, op] = *s.pending;
    double result = 0;
    // Operator の全ての値を case で扱う（default を書かないことで、値を追加したときに
    // コンパイラが -Wswitch で処理漏れを警告してくれる）
    switch (op) {
        case Operator::Add:      result = lhs + s.current; break;
        case Operator::Subtract: result = lhs - s.current; break;
        case Operator::Multiply: result = lhs * s.current; break;
        case Operator::Divide:
            if (s.current == 0) {
                return {s, Error{ErrorCode::DivisionByZero}};
            }
            result = lhs / s.current;
            break;
    }

    // inf / NaN を結果として表示させない
    if (!std::isfinite(result)) {
        return {s, Error{ErrorCode::Overflow}};
    }

    CalculatorState next{};
    next.current = result;
    return {next, DisplayUpdated{result}};
}

// ② inputDigit
inline std::pair<CalculatorState, CalculatorEvent> inputDigit(const CalculatorState& s, int digit) {
    // 入力値の検証：0〜9以外は受け付けない
    if (digit < 0 || digit > 9) {
        return {s, Error{ErrorCode::InvalidDigit}};
    }

    CalculatorState next = s;

    if (!s.enteringNumber) {
        // 新しい数値入力開始
        next.current = digit;
        next.enteringNumber = true;
    } else {
        const double candidate = s.current * 10 + digit;
        // 桁数上限を超える入力は無視（表示は変えない）
        if (candidate >= kMaxInputValue) {
            return {s, DisplayUpdated{s.current}};
        }
        next.current = candidate;
    }

    return {next, DisplayUpdated{next.current}};
}

// ③ inputOperator
inline std::pair<CalculatorState, CalculatorEvent> inputOperator(const CalculatorState& s,
                                                                 Operator op) {
    CalculatorState next = s;

    if (s.pending) {
        // 演算子の連打（2 + * など）は計算せず、演算子を置き換える
        if (!s.enteringNumber) {
            next.pending->op = op;
            return {next, DisplayUpdated{next.current}};
        }

        // 連続計算：保留中の演算を先に計算する
        auto [calculated, event] = calculate(s);
        // 途中計算のエラー（0除算など）を握りつぶさずに返す
        if (std::holds_alternative<Error>(event)) {
            return {s, event};
        }
        next = calculated;
    }

    next.pending = PendingOperation{next.current, op};
    next.enteringNumber = false; // 次の数字は新規入力

    return {next, DisplayUpdated{next.current}};
}

// ④ clear
inline std::pair<CalculatorState, CalculatorEvent> clearAll() {
    return {CalculatorState{}, DisplayUpdated{0}};
}
