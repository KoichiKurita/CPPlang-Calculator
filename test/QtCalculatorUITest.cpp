#include "ui/QtCalculatorUI.h"
#include "app/CalculatorController.h"
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QTest>
#include <gtest/gtest.h>
#include <string>

namespace {

// 実際の Qt の画面（QtCalculatorUI）とコントローラを組み合わせて動かすテスト
class QtCalculatorUITest : public ::testing::Test {
protected:
    QtCalculatorUITest() {
        controller_.run();
        ui_.show();
    }

    std::string display() const {
        return ui_.findChild<QLineEdit*>("display")->text().toStdString();
    }

    // ラベルでボタンを探してクリックする
    void click(const char* label) {
        const QString text = QString::fromUtf8(label);
        for (auto* button : ui_.findChildren<QPushButton*>()) {
            if (button->text() == text) {
                button->click();
                return;
            }
        }
        FAIL() << "button not found: " << label;
    }

    QtCalculatorUI ui_;
    CalculatorController controller_{ui_};
};

} // namespace

TEST_F(QtCalculatorUITest, InitialDisplayIsZero) {
    EXPECT_EQ(display(), "0");
}

TEST_F(QtCalculatorUITest, ButtonCalculation) {
    click("7");
    click("+");
    click("8");
    click("=");

    EXPECT_EQ(display(), "15");
}

TEST_F(QtCalculatorUITest, KeyboardCalculation) {
    // 12.5 * 2 ± Enter → -25
    QTest::keyClicks(&ui_, "12.5*2");
    click("±");
    QTest::keyClick(&ui_, Qt::Key_Return);

    EXPECT_EQ(display(), "-25");
}

TEST_F(QtCalculatorUITest, DecimalEntryIsShownAsTyped) {
    QTest::keyClicks(&ui_, "1.50");

    EXPECT_EQ(display(), "1.50");
}

TEST_F(QtCalculatorUITest, BackspaceAndEscapeKeys) {
    QTest::keyClicks(&ui_, "123");
    QTest::keyClick(&ui_, Qt::Key_Backspace);
    EXPECT_EQ(display(), "12");

    QTest::keyClick(&ui_, Qt::Key_Escape);
    EXPECT_EQ(display(), "0");
}

TEST_F(QtCalculatorUITest, ErrorIsShownInDisplay) {
    QTest::keyClicks(&ui_, "5/0=");

    EXPECT_EQ(display(), "Cannot divide by zero");
}

int main(int argc, char** argv) {
    // 画面のない CI でも動くよう、指定がなければ offscreen プラットフォームを使う
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    const QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
