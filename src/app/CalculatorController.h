#pragma once
#include "../ui/ICalculatorUI.h"

class CalculatorController {
public:
    explicit CalculatorController(ICalculatorUI& ui);
    void run();

private:
    ICalculatorUI& ui_;
};
