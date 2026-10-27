#pragma once
#include "CalculatorState.h"
#include "CalculatorEvent.h"
#include <cmath>
#include <utility>

// 入力できる数値の上限（15桁）。double で誤差なく扱える整数の範囲内に収める
inline constexpr double kMaxInputValue = 1e15;

inline bool isValidOperator(char op) {
    return op == '+' || op == '-' || op == '*' || op == '/';
}

// ① calculate
inline std::pair<CalculatorState, CalculatorEvent>
calculate(const CalculatorState& s) {
    if (!s.hasOp || !s.stored) {
        return { s, Error{"No operation"} };
    }

    double result = 0;
    switch (s.op) {
        case '+': result = *s.stored + s.current; break;
        case '-': result = *s.stored - s.current; break;
        case '*': result = *s.stored * s.current; break;
        case '/':
            if (s.current == 0) {
                return { s, Error{"Division by zero"} };
            }
            result = *s.stored / s.current;
            break;
        default:
            // 想定外の演算子で 0 を返さず、エラーとして明示する
            return { s, Error{"Invalid operator"} };
    }

    // inf / NaN を結果として表示させない
    if (!std::isfinite(result)) {
        return { s, Error{"Overflow"} };
    }

    CalculatorState next{};
    next.current = result;
    next.enteringNumber = false;
    return { next, DisplayUpdated{result} };
}

// ② inputDigit
inline std::pair<CalculatorState, CalculatorEvent>
inputDigit(const CalculatorState& s, int digit) {
    // 入力値の検証：0〜9以外は受け付けない
    if (digit < 0 || digit > 9) {
        return { s, Error{"Invalid digit"} };
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
            return { s, DisplayUpdated{s.current} };
        }
        next.current = candidate;
    }

    return { next, DisplayUpdated{next.current} };
}

// ③ inputOperator
inline std::pair<CalculatorState, CalculatorEvent>
inputOperator(const CalculatorState& s, char op) {
    if (!isValidOperator(op)) {
        return { s, Error{"Invalid operator"} };
    }

    CalculatorState next = s;

    // 演算子の連打（2 + * など）は計算せず、演算子を置き換える
    if (s.hasOp && !s.enteringNumber) {
        next.op = op;
        return { next, DisplayUpdated{next.current} };
    }

    // 連続計算：既に演算子があれば先に計算
    if (s.hasOp && s.stored) {
        auto [calculated, event] = calculate(s);
        // 途中計算のエラー（0除算など）を握りつぶさずに返す
        if (std::holds_alternative<Error>(event)) {
            return { s, event };
        }
        next = calculated;
    }

    next.stored = next.current;
    next.op = op;
    next.hasOp = true;
    next.enteringNumber = false; // 次の数字は新規入力

    return { next, DisplayUpdated{next.current} };
}

// ④ clear
inline std::pair<CalculatorState, CalculatorEvent>
clearAll() {
    return { CalculatorState{}, DisplayUpdated{0} };
}
