#include "CalculatorController.h"
#include "../domain/CalculatorLogic.h"

CalculatorController::CalculatorController(ICalculatorUI& ui)
    : ui_(ui) {}

void CalculatorController::run() {
    // ローカル変数を参照キャプチャすると run() 終了後にダングリング参照になるため、
    // this をキャプチャしてメンバの state_ を使う
    ui_.onInput([this](Input input) { handle(input); });
}

void CalculatorController::handle(const Input& input) {
    std::pair<CalculatorState, CalculatorEvent> result{ state_, Error{"Unknown input"} };

    switch (input.type) {
        case InputType::Digit:    result = inputDigit(state_, input.value); break;
        case InputType::Operator: result = inputOperator(state_, input.op); break;
        case InputType::Equal:    result = calculate(state_);               break;
        case InputType::Clear:    result = clearAll();                      break;
    }

    state_ = result.first;
    ui_.handleEvent(result.second);
}
