#include "CalculatorController.h"
#include "../domain/CalculatorLogic.h"

CalculatorController::CalculatorController(ICalculatorUI& ui) : ui_(ui), store_(nullStore_) {}

CalculatorController::CalculatorController(ICalculatorUI& ui, IHistoryStore& store)
    : ui_(ui), store_(store) {}

void CalculatorController::run() {
    // ローカル変数を参照キャプチャすると run() 終了後にダングリング参照になるため、
    // this をキャプチャしてメンバの state_ を使う
    ui_.onInput([this](const Input& input) { handle(input); });

    // 保存されている履歴を復元する（不正な値は History::restore で取り除かれる）
    history_.restore(store_.load());
    showHistory();
}

void CalculatorController::handle(const Input& input) {
    // 履歴の操作は計算の状態とは別に扱う
    if (input.type == InputType::UseHistory) {
        useHistory(input.value);
        return;
    }
    if (input.type == InputType::ClearHistory) {
        clearHistory();
        return;
    }

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
        case InputType::Expression:
            result = evaluateExpressionInput(state_, input.expression);
            break;
        case InputType::UseHistory:
        case InputType::ClearHistory: break; // 上で処理済み
    }

    const CalculatorState before = state_;
    state_ = result.first;
    ui_.handleEvent(result.second);

    if (const auto* display = std::get_if<DisplayUpdated>(&result.second)) {
        record(input, before, display->value);
    }
}

// 「=」と式の評価が成功したときだけ履歴に記録する
void CalculatorController::record(const Input& input, const CalculatorState& before,
                                  double result) {
    std::string expression;
    if (input.type == InputType::Equal && before.pending) {
        expression = describeOperation(*before.pending, before.current);
    } else if (input.type == InputType::Expression) {
        expression = input.expression;
    } else {
        return;
    }
    history_.add(HistoryEntry{expression, result});
    store_.save(history_.entries());
    showHistory();
}

// 履歴の結果を表示中の値にする（続けて計算に使える）
void CalculatorController::useHistory(int index) {
    const auto& entries = history_.entries();
    // UI から渡された位置は範囲を確認してから使う
    if (index < 0 || static_cast<std::size_t>(index) >= entries.size()) {
        return;
    }
    state_ = CalculatorState{};
    state_.current = entries[static_cast<std::size_t>(index)].result;
    ui_.handleEvent(DisplayUpdated{state_.current});
}

void CalculatorController::clearHistory() {
    history_.clear();
    store_.save(history_.entries());
    showHistory();
}

void CalculatorController::showHistory() {
    ui_.showHistory(history_.entries());
}
