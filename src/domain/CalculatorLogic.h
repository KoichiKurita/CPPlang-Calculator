#pragma once
#include "CalculatorState.h"
#include "CalculatorEvent.h"
#include <utility>

// ① 先に calculate を定義
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
            if (s.current == 0)
                return { s, Error{"Division by zero"} };
            result = *s.stored / s.current;
            break;
    }

    CalculatorState next{ result, {}, 0, false };
    next.enteringNumber = false;
    return { next, DisplayUpdated{result} };
}

// ② inputDigit
inline std::pair<CalculatorState, CalculatorEvent>
inputDigit(const CalculatorState& s, int digit) {
    CalculatorState next = s;

    if (!s.enteringNumber) {
        // 新しい数値入力開始
        next.current = digit;
        next.enteringNumber = true;
    } else {
        // 桁追加
        next.current = s.current * 10 + digit;
    }

    return { next, DisplayUpdated{next.current} };
}


// ③ inputOperator（calculate を使う）
inline std::pair<CalculatorState, CalculatorEvent>
inputOperator(const CalculatorState& s, char op) {
    CalculatorState next = s;

    // 連続計算対応：既に演算子があれば先に計算
    if (s.hasOp && s.stored) {
        auto [calculated, event] = calculate(s);
        next = calculated;
    }

    next.stored = next.current;
    next.op = op;
    next.hasOp = true;

    // ★ 次の数字は新規入力
    next.enteringNumber = false;

    // ★ current は 0 にしない
    // ★ 表示も変更しない
    return { next, DisplayUpdated{next.current} };
}


// ④ clear
inline std::pair<CalculatorState, CalculatorEvent>
clearAll() {
    CalculatorState next{};
    return { next, DisplayUpdated{0} };
}
