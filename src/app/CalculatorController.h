#pragma once
#include "../domain/CalculatorState.h"
#include "../ui/ICalculatorUI.h"

class CalculatorController {
public:
    explicit CalculatorController(ICalculatorUI& ui);
    void run();

private:
    void handle(const Input& input);

    ICalculatorUI& ui_;
    CalculatorState state_{}; // 状態はコントローラが所有する（寿命 = コントローラの寿命）
};
