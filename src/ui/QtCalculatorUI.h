#pragma once
#include "ICalculatorUI.h"
#include <QLineEdit>
#include <QListWidget>
#include <QWidget>

class QKeyEvent;

class QtCalculatorUI : public QWidget, public ICalculatorUI {
    Q_OBJECT
public:
    QtCalculatorUI();

    void onInput(std::function<void(Input)> handler) override;
    void handleEvent(const CalculatorEvent& event) override;
    void showHistory(const std::vector<HistoryEntry>& entries) override;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    QLineEdit* expressionInput_; // 式の入力欄（例: (1+2)*3）
    QLineEdit* display_;
    QListWidget* historyList_; // 計算履歴（新しいものが先頭）
    std::function<void(Input)> handler_;

    void emitInput(const Input& input);
};
