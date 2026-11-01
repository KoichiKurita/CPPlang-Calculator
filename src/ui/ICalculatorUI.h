#pragma once

#include "../domain/CalculatorEvent.h"
#include "../domain/CalculatorState.h"
#include "../domain/History.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

enum class InputType : std::uint8_t {
    Digit,
    Operator,
    Equal,
    Clear,
    DecimalPoint,
    ToggleSign,
    Backspace,
    Expression,   // 式の入力欄で Enter が押された（式は Input::expression）
    UseHistory,   // 履歴の項目が選ばれた（位置は Input::value。0 が最新）
    ClearHistory, // 履歴のクリア
};

struct Input {
    InputType type{};
    int value{0};
    Operator op{Operator::Add};
    // 式の入力欄の文字列（InputType::Expression のときに使う）
    // {} は Input{InputType::Clear} のように省略して初期化したときの GCC の
    // -Wmissing-field-initializers を防ぐため（DisplayUpdated::entry と同じ理由）。
    // NOLINTNEXTLINE(readability-redundant-member-init)
    std::string expression{};
};

class ICalculatorUI {
public:
    ICalculatorUI() = default;
    virtual ~ICalculatorUI() = default;

    // インターフェースはコピー・ムーブしない（派生クラスのスライシングを防ぐ）
    ICalculatorUI(const ICalculatorUI&) = delete;
    ICalculatorUI& operator=(const ICalculatorUI&) = delete;
    ICalculatorUI(ICalculatorUI&&) = delete;
    ICalculatorUI& operator=(ICalculatorUI&&) = delete;

    virtual void onInput(std::function<void(Input)> handler) = 0;
    virtual void handleEvent(const CalculatorEvent& event) = 0;
    // 履歴の表示を更新する（新しいものが先頭）
    virtual void showHistory(const std::vector<HistoryEntry>& entries) = 0;
};
