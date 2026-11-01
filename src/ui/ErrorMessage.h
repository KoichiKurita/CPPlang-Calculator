#pragma once
#include "../domain/CalculatorEvent.h"

// エラーの種類を表示文言に変換する（Qt 非依存の純粋関数としてテスト可能にする）
inline const char* errorMessage(ErrorCode code) {
    switch (code) {
        case ErrorCode::NoOperation:    return "No operation";
        case ErrorCode::DivisionByZero: return "Cannot divide by zero";
        case ErrorCode::Overflow:       return "Overflow";
        case ErrorCode::InvalidDigit:   return "Invalid input";
    }
    return "Error"; // 列挙型の範囲外の値（到達不能） GCOVR_EXCL_LINE
}
