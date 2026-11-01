#include "app/CalculatorController.h"
#include <gtest/gtest.h>

namespace {

// Qt を使わずにコントローラを検証するためのテスト用UI（Fake）
class FakeUI : public ICalculatorUI {
public:
    void onInput(std::function<void(Input)> handler) override { handler_ = std::move(handler); }
    void handleEvent(const CalculatorEvent& event) override { lastEvent_ = event; }

    void press(Input input) { handler_(input); }
    double display() const { return std::get<DisplayUpdated>(lastEvent_).value; }
    bool showsError() const { return std::holds_alternative<Error>(lastEvent_); }

private:
    std::function<void(Input)> handler_;
    CalculatorEvent lastEvent_{DisplayUpdated{0}};
};

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
