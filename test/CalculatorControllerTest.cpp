#include "app/CalculatorController.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

// Qt を使わずにコントローラを検証するためのテスト用UI（Fake）
class FakeUI : public ICalculatorUI {
public:
    void onInput(std::function<void(Input)> handler) override { handler_ = std::move(handler); }
    void handleEvent(const CalculatorEvent& event) override { lastEvent_ = event; }
    void showHistory(const std::vector<HistoryEntry>& entries) override { history_ = entries; }

    void press(Input input) { handler_(input); }
    double display() const { return std::get<DisplayUpdated>(lastEvent_).value; }
    bool showsError() const { return std::holds_alternative<Error>(lastEvent_); }
    const std::vector<HistoryEntry>& history() const { return history_; }

private:
    std::function<void(Input)> handler_;
    CalculatorEvent lastEvent_{DisplayUpdated{0}};
    std::vector<HistoryEntry> history_;
};

// テスト用の保存先（メモリ上に保存する）
class FakeHistoryStore : public IHistoryStore {
public:
    std::vector<HistoryEntry> load() override { return stored; }
    void save(const std::vector<HistoryEntry>& entries) override {
        stored = entries;
        ++saveCount;
    }

    std::vector<HistoryEntry> stored;
    int saveCount = 0;
};

Input expressionInput(const std::string& text) {
    Input input{InputType::Expression};
    input.expression = text;
    return input;
}

} // namespace

TEST(CalculatorControllerTest, KeepsStateAfterRunReturns) {
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run(); // run() が戻った後も入力を処理できること

    ui.press({InputType::Digit, 1});
    ui.press({InputType::Operator, 0, Operator::Add});
    ui.press({InputType::Digit, 2});
    ui.press({InputType::Equal});

    EXPECT_DOUBLE_EQ(ui.display(), 3.0);
}

TEST(CalculatorControllerTest, ClearResetsDisplay) {
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run();

    ui.press({InputType::Digit, 7});
    ui.press({InputType::Clear});

    EXPECT_DOUBLE_EQ(ui.display(), 0.0);
}

TEST(CalculatorControllerTest, ErrorIsReplacedByNextInput) {
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run();

    // 5 / 0 = → エラー表示
    ui.press({InputType::Digit, 5});
    ui.press({InputType::Operator, 0, Operator::Divide});
    ui.press({InputType::Digit, 0});
    ui.press({InputType::Equal});
    ASSERT_TRUE(ui.showsError());

    // 次の入力で表示が数値に戻る
    ui.press({InputType::Clear});
    ui.press({InputType::Digit, 3});
    EXPECT_FALSE(ui.showsError());
    EXPECT_DOUBLE_EQ(ui.display(), 3.0);
}

TEST(CalculatorControllerTest, DecimalAndNegativeCalculation) {
    // 12.5 * -2 = -25（-2 は 2 を入力してから ± で反転）
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run();

    ui.press({InputType::Digit, 1});
    ui.press({InputType::Digit, 2});
    ui.press({InputType::DecimalPoint});
    ui.press({InputType::Digit, 5});
    ui.press({InputType::Operator, 0, Operator::Multiply});
    ui.press({InputType::Digit, 2});
    ui.press({InputType::ToggleSign});
    ui.press({InputType::Equal});

    EXPECT_DOUBLE_EQ(ui.display(), -25.0);
}

TEST(CalculatorControllerTest, BackspaceEditsEntry) {
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run();

    ui.press({InputType::Digit, 4});
    ui.press({InputType::Digit, 2});
    ui.press({InputType::Backspace});

    EXPECT_DOUBLE_EQ(ui.display(), 4.0);
}

TEST(CalculatorControllerTest, ExpressionResultCanBeUsedInNextCalculation) {
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run();

    Input expression{InputType::Expression};
    expression.expression = "(1+2)*3";
    ui.press(expression);
    EXPECT_DOUBLE_EQ(ui.display(), 9.0);

    // 結果に続けてボタンで計算できる：9 + 1 = 10
    ui.press({InputType::Operator, 0, Operator::Add});
    ui.press({InputType::Digit, 1});
    ui.press({InputType::Equal});
    EXPECT_DOUBLE_EQ(ui.display(), 10.0);
}

TEST(CalculatorControllerTest, InvalidExpressionShowsError) {
    FakeUI ui;
    CalculatorController controller(ui);
    controller.run();

    Input expression{InputType::Expression};
    expression.expression = "1+";
    ui.press(expression);

    EXPECT_TRUE(ui.showsError());
}

// ---- 計算履歴 ----

TEST(CalculatorControllerTest, ButtonCalculationIsRecorded) {
    FakeUI ui;
    FakeHistoryStore store;
    CalculatorController controller(ui, store);
    controller.run();

    ui.press({InputType::Digit, 2});
    ui.press({InputType::Operator, 0, Operator::Add});
    ui.press({InputType::Digit, 3});
    ui.press({InputType::Equal});

    ASSERT_EQ(ui.history().size(), 1U);
    EXPECT_EQ(ui.history()[0], (HistoryEntry{"2 + 3", 5}));
    EXPECT_EQ(store.stored, ui.history()); // 追加のたびに保存される
}

TEST(CalculatorControllerTest, ExpressionIsRecorded) {
    FakeUI ui;
    FakeHistoryStore store;
    CalculatorController controller(ui, store);
    controller.run();

    ui.press(expressionInput("(1+2)*3"));

    ASSERT_EQ(ui.history().size(), 1U);
    EXPECT_EQ(ui.history()[0], (HistoryEntry{"(1+2)*3", 9}));
}

TEST(CalculatorControllerTest, ErrorsAndIntermediateResultsAreNotRecorded) {
    FakeUI ui;
    FakeHistoryStore store;
    CalculatorController controller(ui, store);
    controller.run();

    // 連続計算の途中結果（2 + 3 + の時点の 5）は記録しない
    ui.press({InputType::Digit, 2});
    ui.press({InputType::Operator, 0, Operator::Add});
    ui.press({InputType::Digit, 3});
    ui.press({InputType::Operator, 0, Operator::Add});
    // エラーは記録しない
    ui.press(expressionInput("1/0"));
    ui.press({InputType::Clear});
    ui.press({InputType::Equal}); // No operation

    EXPECT_TRUE(ui.history().empty());
    EXPECT_EQ(store.saveCount, 0);
}

TEST(CalculatorControllerTest, HistoryIsRestoredOnRun) {
    FakeUI ui;
    FakeHistoryStore store;
    store.stored = {{"1 + 1", 2}, {"", 3}}; // 2件目は不正な値（復元時に取り除かれる）
    CalculatorController controller(ui, store);
    controller.run();

    ASSERT_EQ(ui.history().size(), 1U);
    EXPECT_EQ(ui.history()[0], (HistoryEntry{"1 + 1", 2}));
}

TEST(CalculatorControllerTest, HistoryResultCanBeReused) {
    FakeUI ui;
    FakeHistoryStore store;
    store.stored = {{"6 * 7", 42}};
    CalculatorController controller(ui, store);
    controller.run();

    ui.press({InputType::UseHistory, 0});
    EXPECT_DOUBLE_EQ(ui.display(), 42.0);

    // 続けて計算できる：42 / 2 = 21
    ui.press({InputType::Operator, 0, Operator::Divide});
    ui.press({InputType::Digit, 2});
    ui.press({InputType::Equal});
    EXPECT_DOUBLE_EQ(ui.display(), 21.0);
}

TEST(CalculatorControllerTest, InvalidHistoryIndexIsIgnored) {
    FakeUI ui;
    FakeHistoryStore store;
    store.stored = {{"6 * 7", 42}};
    CalculatorController controller(ui, store);
    controller.run();

    ui.press({InputType::Digit, 5});
    ui.press({InputType::UseHistory, 1});
    ui.press({InputType::UseHistory, -1});

    EXPECT_DOUBLE_EQ(ui.display(), 5.0);
}

TEST(CalculatorControllerTest, ClearHistory) {
    FakeUI ui;
    FakeHistoryStore store;
    store.stored = {{"6 * 7", 42}};
    CalculatorController controller(ui, store);
    controller.run();

    ui.press({InputType::ClearHistory});

    EXPECT_TRUE(ui.history().empty());
    EXPECT_TRUE(store.stored.empty());
}
