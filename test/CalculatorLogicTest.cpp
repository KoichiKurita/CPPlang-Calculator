#include <gtest/gtest.h>
#include "domain/CalculatorLogic.h"

namespace {

double displayed(const CalculatorEvent& e) {
    return std::get<DisplayUpdated>(e).value;
}

bool isError(const CalculatorEvent& e) {
    return std::holds_alternative<Error>(e);
}

} // namespace

TEST(CalculatorLogicTest, Addition) {
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, '+');
    auto [s3, e3] = inputDigit(s2, 3);
    auto [s4, e4] = calculate(s3);

    EXPECT_DOUBLE_EQ(displayed(e4), 5.0);
}

TEST(CalculatorLogicTest, ContinuousCalculation) {
    // 2 + 3 + 4 = 9
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, '+');
    auto [s3, e3] = inputDigit(s2, 3);
    auto [s4, e4] = inputOperator(s3, '+');
    EXPECT_DOUBLE_EQ(displayed(e4), 5.0); // 途中結果が表示される
    auto [s5, e5] = inputDigit(s4, 4);
    auto [s6, e6] = calculate(s5);

    EXPECT_DOUBLE_EQ(displayed(e6), 9.0);
}

TEST(CalculatorLogicTest, MultiDigitInput) {
    auto [s1, e1] = inputDigit({}, 1);
    auto [s2, e2] = inputDigit(s1, 2);
    auto [s3, e3] = inputDigit(s2, 3);

    EXPECT_DOUBLE_EQ(displayed(e3), 123.0);
}

TEST(CalculatorLogicTest, DivisionByZeroReturnsErrorAndKeepsState) {
    auto [s1, e1] = inputDigit({}, 5);
    auto [s2, e2] = inputOperator(s1, '/');
    auto [s3, e3] = inputDigit(s2, 0);
    auto [s4, e4] = calculate(s3);

    EXPECT_TRUE(isError(e4));
    EXPECT_DOUBLE_EQ(s4.current, s3.current);
}

TEST(CalculatorLogicTest, EqualWithoutOperatorIsError) {
    auto [s1, e1] = inputDigit({}, 7);
    auto [s2, e2] = calculate(s1);

    EXPECT_TRUE(isError(e2));
}

TEST(CalculatorLogicTest, ClearResetsState) {
    auto [s2, e2] = clearAll();

    EXPECT_DOUBLE_EQ(displayed(e2), 0.0);
    EXPECT_FALSE(s2.hasOp);
    EXPECT_FALSE(s2.stored.has_value());
}

TEST(CalculatorLogicTest, DivisionByZeroInChainIsNotSwallowed) {
    // 5 / 0 + → エラーになるべき
    auto [s1, e1] = inputDigit({}, 5);
    auto [s2, e2] = inputOperator(s1, '/');
    auto [s3, e3] = inputDigit(s2, 0);
    auto [s4, e4] = inputOperator(s3, '+');

    EXPECT_TRUE(isError(e4));
}

TEST(CalculatorLogicTest, RepeatedOperatorReplacesPrevious) {
    // 2 + * 3 = 6 （+ は * に置き換わる）
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, '+');
    auto [s3, e3] = inputOperator(s2, '*');
    auto [s4, e4] = inputDigit(s3, 3);
    auto [s5, e5] = calculate(s4);

    EXPECT_DOUBLE_EQ(displayed(e3), 2.0);
    EXPECT_DOUBLE_EQ(displayed(e5), 6.0);
}

TEST(CalculatorLogicTest, InvalidDigitIsRejected) {
    auto [s1, e1] = inputDigit({}, 10);
    auto [s2, e2] = inputDigit({}, -1);

    EXPECT_TRUE(isError(e1));
    EXPECT_TRUE(isError(e2));
}

TEST(CalculatorLogicTest, InvalidOperatorIsRejected) {
    auto [s1, e1] = inputDigit({}, 1);
    auto [s2, e2] = inputOperator(s1, '%');

    EXPECT_TRUE(isError(e2));
}

TEST(CalculatorLogicTest, InputIsLimitedTo15Digits) {
    CalculatorState s{};
    for (int i = 0; i < 100; ++i) {
        s = inputDigit(s, 9).first;
    }

    EXPECT_DOUBLE_EQ(s.current, 999'999'999'999'999.0);
}

TEST(CalculatorLogicTest, OverflowIsReportedAsError) {
    CalculatorState s{};
    s.current = 1e300;
    s.stored = 1e300;
    s.op = '*';
    s.hasOp = true;

    auto [s1, e1] = calculate(s);

    EXPECT_TRUE(isError(e1));
}
