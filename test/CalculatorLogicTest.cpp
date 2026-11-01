#include "domain/CalculatorLogic.h"
#include <gtest/gtest.h>

namespace {

double displayed(const CalculatorEvent& e) {
    return std::get<DisplayUpdated>(e).value;
}

bool isError(const CalculatorEvent& e) {
    return std::holds_alternative<Error>(e);
}

ErrorCode errorOf(const CalculatorEvent& e) {
    return std::get<Error>(e).code;
}

} // namespace

TEST(CalculatorLogicTest, Addition) {
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, Operator::Add);
    auto [s3, e3] = inputDigit(s2, 3);
    auto [s4, e4] = calculate(s3);

    EXPECT_DOUBLE_EQ(displayed(e4), 5.0);
}

TEST(CalculatorLogicTest, Subtraction) {
    auto [s1, e1] = inputDigit({}, 9);
    auto [s2, e2] = inputOperator(s1, Operator::Subtract);
    auto [s3, e3] = inputDigit(s2, 4);
    auto [s4, e4] = calculate(s3);

    EXPECT_DOUBLE_EQ(displayed(e4), 5.0);
}

TEST(CalculatorLogicTest, Multiplication) {
    auto [s1, e1] = inputDigit({}, 6);
    auto [s2, e2] = inputOperator(s1, Operator::Multiply);
    auto [s3, e3] = inputDigit(s2, 7);
    auto [s4, e4] = calculate(s3);

    EXPECT_DOUBLE_EQ(displayed(e4), 42.0);
}

TEST(CalculatorLogicTest, Division) {
    auto [s1, e1] = inputDigit({}, 8);
    auto [s2, e2] = inputOperator(s1, Operator::Divide);
    auto [s3, e3] = inputDigit(s2, 2);
    auto [s4, e4] = calculate(s3);

    EXPECT_DOUBLE_EQ(displayed(e4), 4.0);
}

TEST(CalculatorLogicTest, ContinuousCalculation) {
    // 2 + 3 + 4 = 9
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, Operator::Add);
    auto [s3, e3] = inputDigit(s2, 3);
    auto [s4, e4] = inputOperator(s3, Operator::Add);
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
    auto [s2, e2] = inputOperator(s1, Operator::Divide);
    auto [s3, e3] = inputDigit(s2, 0);
    auto [s4, e4] = calculate(s3);

    ASSERT_TRUE(isError(e4));
    EXPECT_EQ(errorOf(e4), ErrorCode::DivisionByZero);
    EXPECT_DOUBLE_EQ(s4.current, s3.current);
}

TEST(CalculatorLogicTest, DivisionByZeroInChainIsNotSwallowed) {
    // 5 / 0 + → エラーになるべき
    auto [s1, e1] = inputDigit({}, 5);
    auto [s2, e2] = inputOperator(s1, Operator::Divide);
    auto [s3, e3] = inputDigit(s2, 0);
    auto [s4, e4] = inputOperator(s3, Operator::Add);

    ASSERT_TRUE(isError(e4));
    EXPECT_EQ(errorOf(e4), ErrorCode::DivisionByZero);
}

TEST(CalculatorLogicTest, RepeatedOperatorReplacesPrevious) {
    // 2 + * 3 = 6 （+ は * に置き換わる）
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, Operator::Add);
    auto [s3, e3] = inputOperator(s2, Operator::Multiply);
    auto [s4, e4] = inputDigit(s3, 3);
    auto [s5, e5] = calculate(s4);

    EXPECT_DOUBLE_EQ(displayed(e3), 2.0);
    EXPECT_DOUBLE_EQ(displayed(e5), 6.0);
}

TEST(CalculatorLogicTest, EqualWithoutOperatorIsError) {
    auto [s1, e1] = inputDigit({}, 7);
    auto [s2, e2] = calculate(s1);

    ASSERT_TRUE(isError(e2));
    EXPECT_EQ(errorOf(e2), ErrorCode::NoOperation);
}

TEST(CalculatorLogicTest, InvalidDigitIsRejected) {
    auto [s1, e1] = inputDigit({}, 10);
    auto [s2, e2] = inputDigit({}, -1);

    ASSERT_TRUE(isError(e1));
    ASSERT_TRUE(isError(e2));
    EXPECT_EQ(errorOf(e1), ErrorCode::InvalidDigit);
    EXPECT_EQ(errorOf(e2), ErrorCode::InvalidDigit);
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
    s.pending = PendingOperation{1e300, Operator::Multiply};

    auto [s1, e1] = calculate(s);

    ASSERT_TRUE(isError(e1));
    EXPECT_EQ(errorOf(e1), ErrorCode::Overflow);
}

TEST(CalculatorLogicTest, OperatorAfterResultStartsNewCalculation) {
    // 2 + 3 = 5、続けて * 4 = 20
    auto [s1, e1] = inputDigit({}, 2);
    auto [s2, e2] = inputOperator(s1, Operator::Add);
    auto [s3, e3] = inputDigit(s2, 3);
    auto [s4, e4] = calculate(s3);
    auto [s5, e5] = inputOperator(s4, Operator::Multiply);
    auto [s6, e6] = inputDigit(s5, 4);
    auto [s7, e7] = calculate(s6);

    EXPECT_DOUBLE_EQ(displayed(e7), 20.0);
}

TEST(CalculatorLogicTest, ClearResetsState) {
    auto [s, e] = clearAll();

    EXPECT_DOUBLE_EQ(displayed(e), 0.0);
    EXPECT_FALSE(s.pending.has_value());
    EXPECT_FALSE(s.enteringNumber);
}
