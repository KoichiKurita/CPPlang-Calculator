#pragma once
#include "../domain/CalculatorState.h"
#include "../domain/History.h"
#include "../ui/ICalculatorUI.h"
#include "IHistoryStore.h"

class CalculatorController {
public:
    // 履歴を保存しない（テストや、保存先がない環境向け）
    explicit CalculatorController(ICalculatorUI& ui);
    // 履歴を store に保存・復元する
    CalculatorController(ICalculatorUI& ui, IHistoryStore& store);

    void run();

private:
    void handle(const Input& input);
    void record(const Input& input, const CalculatorState& before, double result);
    void useHistory(int index);
    void clearHistory();
    void showHistory();

    ICalculatorUI& ui_;
    NullHistoryStore nullStore_; // store_ より先に宣言する（初期化の順序のため）
    IHistoryStore& store_;
    CalculatorState state_{}; // 状態はコントローラが所有する（寿命 = コントローラの寿命）
    History history_;
};
