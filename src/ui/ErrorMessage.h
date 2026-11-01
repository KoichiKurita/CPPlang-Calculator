#pragma once
#include "../domain/CalculatorEvent.h"
#include <string>

// エラーの種類を表示文言に変換する（Qt 非依存の純粋関数としてテスト可能にする）
inline const char* errorMessage(ErrorCode code) {
    switch (code) {
        case ErrorCode::NoOperation:       return "No operation";
        case ErrorCode::DivisionByZero:    return "Cannot divide by zero";
        case ErrorCode::Overflow:          return "Overflow";
        case ErrorCode::InvalidDigit:      return "Invalid input";
        case ErrorCode::InvalidExpression: return "Syntax error";
        case ErrorCode::ExpressionTooLong: return "Expression too long";
        case ErrorCode::ExpressionTooDeep: return "Expression too deeply nested";
    }
    return "Error"; // 列挙型の範囲外の値（到達不能） GCOVR_EXCL_LINE
}

// 表示欄に出す文字列。式の書き方のエラーには位置（1 始まり）を付ける
inline std::string errorText(const Error& error) {
    std::string text = errorMessage(error.code);
    if (error.code == ErrorCode::InvalidExpression) {
        text += " at " + std::to_string(error.position + 1);
    }
    return text;
}
