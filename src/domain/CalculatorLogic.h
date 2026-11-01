#pragma once
#include "CalculatorEvent.h"
#include "CalculatorState.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <locale>
#include <sstream>
#include <string>
#include <utility>

// 入力できる桁数の上限（15桁）。double で誤差なく扱える10進桁数に合わせる
inline constexpr std::size_t kMaxDigits = 15;

// 入力中の文字列を数値に変換する（環境のロケールに依存せず "." を小数点として扱う）
inline double parseEntry(const std::string& entry) {
    std::istringstream iss(entry);
    iss.imbue(std::locale::classic());
    double value = 0;
    iss >> value;
    return value;
}

inline std::size_t countDigits(const std::string& entry) {
    return static_cast<std::size_t>(std::count_if(
        entry.begin(), entry.end(), [](unsigned char c) { return std::isdigit(c) != 0; }));
}

// 入力中の文字列を更新した状態と、表示イベントを作る
inline std::pair<CalculatorState, CalculatorEvent> withEntry(CalculatorState s, std::string entry) {
    s.current = parseEntry(entry);
    s.entry = std::move(entry);
    return {s, DisplayUpdated{s.current, s.entry}};
}

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

    const char c = static_cast<char>('0' + digit);
    std::string entry = s.entry;

    if (entry.empty() || entry == "0") {
        // 新しい数値の入力開始、または先頭の 0 を置き換える
        entry = std::string(1, c);
    } else if (entry == "-0") {
        entry = std::string("-") + c;
    } else if (countDigits(entry) >= kMaxDigits) {
        // 桁数上限を超える入力は無視（表示は変えない）
        return {s, DisplayUpdated{s.current, s.entry}};
    } else {
        entry += c;
    }

    return withEntry(s, std::move(entry));
}

// ③ inputDecimalPoint
inline std::pair<CalculatorState, CalculatorEvent> inputDecimalPoint(const CalculatorState& s) {
    if (s.entry.empty()) {
        // 数値の入力前に "." が押されたら "0." から始める
        return withEntry(s, "0.");
    }
    if (s.entry.find('.') != std::string::npos) {
        // 小数点は1つまで（2回目は無視）
        return {s, DisplayUpdated{s.current, s.entry}};
    }
    return withEntry(s, s.entry + ".");
}

// ④ toggleSign（±）
inline std::pair<CalculatorState, CalculatorEvent> toggleSign(const CalculatorState& s) {
    if (s.entry.empty()) {
        // 計算結果などの表示中の値を反転する
        CalculatorState next = s;
        next.current = -s.current;
        return {next, DisplayUpdated{next.current}};
    }
    if (s.entry.front() == '-') {
        return withEntry(s, s.entry.substr(1));
    }
    return withEntry(s, "-" + s.entry);
}

// ⑤ backspace（⌫）：入力中の数値の末尾を1文字削除する
inline std::pair<CalculatorState, CalculatorEvent> backspace(const CalculatorState& s) {
    if (s.entry.empty()) {
        // 計算結果は編集できないので何もしない
        return {s, DisplayUpdated{s.current}};
    }
    std::string entry = s.entry.substr(0, s.entry.size() - 1);
    if (entry.empty() || entry == "-") {
        entry = "0";
    }
    return withEntry(s, std::move(entry));
}

// ⑥ inputOperator
inline std::pair<CalculatorState, CalculatorEvent> inputOperator(const CalculatorState& s,
                                                                 Operator op) {
    CalculatorState next = s;

    if (s.pending) {
        // 演算子の連打（2 + * など）は計算せず、演算子を置き換える
        if (s.entry.empty()) {
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
    next.entry.clear(); // 次の数字は新規入力

    return {next, DisplayUpdated{next.current}};
}

// ⑦ clear
inline std::pair<CalculatorState, CalculatorEvent> clearAll() {
    return {CalculatorState{}, DisplayUpdated{0}};
}
