#include "CalculatorController.h"
#include "../domain/CalculatorLogic.h"

CalculatorController::CalculatorController(ICalculatorUI& ui) : ui_(ui) {}

void CalculatorController::run() {
    // ローカル変数を参照キャプチャすると run() 終了後にダングリング参照になるため、
    // this をキャプチャしてメンバの state_ を使う
    ui_.onInput([this](Input input) { handle(input); });
}

void CalculatorController::handle(const Input& input) {
    // 初期値は「何も変えない」。switch で InputType の全ての値を扱う
    std::pair<CalculatorState, CalculatorEvent> result{state_, DisplayUpdated{state_.current}};

    switch (input.type) {
        case InputType::Digit:        result = inputDigit(state_, input.value); break;
        case InputType::Operator:     result = inputOperator(state_, input.op); break;
        case InputType::Equal:        result = calculate(state_); break;
        case InputType::Clear:        result = clearAll(); break;
        case InputType::DecimalPoint: result = inputDecimalPoint(state_); break;
        case InputType::ToggleSign:   result = toggleSign(state_); break;
        case InputType::Backspace:    result = backspace(state_); break;
    }

    state_ = result.first;
    ui_.handleEvent(result.second);
}
