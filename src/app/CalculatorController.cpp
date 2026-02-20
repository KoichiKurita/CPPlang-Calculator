#include "CalculatorController.h"
#include "../domain/CalculatorLogic.h"

CalculatorController::CalculatorController(ICalculatorUI& ui)
    : ui_(ui) {}

void CalculatorController::run() {
    CalculatorState state;

    ui_.onInput([&](Input input) {
        std::pair<CalculatorState, CalculatorEvent> result;

        switch (input.type) {
            case InputType::Digit:
                result = inputDigit(state, input.value);
                break;
            case InputType::Operator:
                result = inputOperator(state, input.op);
                break;
            case InputType::Equal:
                result = calculate(state);
                break;
            case InputType::Clear:
                result = clearAll();
                break;
        }

        state = result.first;
        ui_.handleEvent(result.second);
    });
}
