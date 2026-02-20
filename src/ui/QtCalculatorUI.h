#pragma once
#include "ICalculatorUI.h"
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>

class QtCalculatorUI : public QWidget, public ICalculatorUI {
    Q_OBJECT
public:
    QtCalculatorUI();

    void onInput(std::function<void(Input)> handler) override;
    void handleEvent(const CalculatorEvent& event) override;

private:
    QLineEdit* display_;
    std::function<void(Input)> handler_;

    void emitDigit(int d);
    void emitOp(char op);
    void emitEqual();
};
