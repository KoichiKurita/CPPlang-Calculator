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
    ui.press({InputType::Operator, 0, '+'});
    ui.press({InputType::Digit, 2});
    ui.press({InputType::Equal});

    EXPECT_DOUBLE_EQ(ui.display(), 3.0);
}
