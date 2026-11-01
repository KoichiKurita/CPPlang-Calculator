#include "domain/Expression.h"
#include <gtest/gtest.h>
#include <string>

namespace {

double valueOf(const std::string& input) {
    const auto result = evaluateExpression(input);
    EXPECT_TRUE(std::holds_alternative<double>(result)) << "input: " << input;
    return std::holds_alternative<double>(result) ? std::get<double>(result) : 0.0;
}

Error errorOf(const std::string& input, std::size_t maxLength = kMaxExpressionLength) {
    const auto result = evaluateExpression(input, maxLength);
    EXPECT_TRUE(std::holds_alternative<Error>(result)) << "input: " << input;
    return std::holds_alternative<Error>(result) ? std::get<Error>(result) : Error{};
}

} // namespace

// ---- 正常系 ----

TEST(ExpressionTest, Numbers) {
    EXPECT_DOUBLE_EQ(valueOf("42"), 42.0);
    EXPECT_DOUBLE_EQ(valueOf("3.5"), 3.5);
    EXPECT_DOUBLE_EQ(valueOf(".5"), 0.5);
    EXPECT_DOUBLE_EQ(valueOf("5."), 5.0);
}

TEST(ExpressionTest, OperatorPrecedence) {
    EXPECT_DOUBLE_EQ(valueOf("2+3*4"), 14.0);
    EXPECT_DOUBLE_EQ(valueOf("2*3+4"), 10.0);
    EXPECT_DOUBLE_EQ(valueOf("8-6/2"), 5.0);
}

TEST(ExpressionTest, Parentheses) {
    EXPECT_DOUBLE_EQ(valueOf("(1+2)*3"), 9.0);
    EXPECT_DOUBLE_EQ(valueOf("((2))"), 2.0);
    EXPECT_DOUBLE_EQ(valueOf("2*(3+(4-1))"), 12.0);
}

TEST(ExpressionTest, LeftAssociativity) {
    EXPECT_DOUBLE_EQ(valueOf("10-4-3"), 3.0);   // (10-4)-3
    EXPECT_DOUBLE_EQ(valueOf("100/10/2"), 5.0); // (100/10)/2
}

TEST(ExpressionTest, UnaryOperators) {
    EXPECT_DOUBLE_EQ(valueOf("-(2+3)"), -5.0);
    EXPECT_DOUBLE_EQ(valueOf("--5"), 5.0);
    EXPECT_DOUBLE_EQ(valueOf("+5"), 5.0);
    EXPECT_DOUBLE_EQ(valueOf("2*-3"), -6.0);
}

TEST(ExpressionTest, WhitespaceIsIgnored) {
    EXPECT_DOUBLE_EQ(valueOf(" 1 +\t2 "), 3.0);
}

TEST(ExpressionTest, DecimalArithmetic) {
    EXPECT_NEAR(valueOf("0.1+0.2"), 0.3, 1e-12);
}

// ---- 異常系（位置は 0 始まり） ----

TEST(ExpressionTest, EmptyInputIsSyntaxError) {
    const Error e = errorOf("");
    EXPECT_EQ(e.code, ErrorCode::InvalidExpression);
    EXPECT_EQ(e.position, 0U);
}

TEST(ExpressionTest, MissingOperandIsSyntaxError) {
    const Error e = errorOf("1+");
    EXPECT_EQ(e.code, ErrorCode::InvalidExpression);
    EXPECT_EQ(e.position, 2U);
}

TEST(ExpressionTest, LeadingBinaryOperatorIsSyntaxError) {
    const Error e = errorOf("*2");
    EXPECT_EQ(e.code, ErrorCode::InvalidExpression);
    EXPECT_EQ(e.position, 0U);
}

TEST(ExpressionTest, MissingCloseParenthesis) {
    const Error e = errorOf("(1+2");
    EXPECT_EQ(e.code, ErrorCode::InvalidExpression);
    EXPECT_EQ(e.position, 4U);
}

TEST(ExpressionTest, ExtraCloseParenthesis) {
    const Error e = errorOf("1+2)");
    EXPECT_EQ(e.code, ErrorCode::InvalidExpression);
    EXPECT_EQ(e.position, 3U);
}

TEST(ExpressionTest, InvalidCharacterIsRejected) {
    const Error e = errorOf("1 $ 2");
    EXPECT_EQ(e.code, ErrorCode::InvalidExpression);
    EXPECT_EQ(e.position, 2U);
}

TEST(ExpressionTest, InvalidNumbersAreRejected) {
    EXPECT_EQ(errorOf("1.2.3").position, 0U);
    EXPECT_EQ(errorOf("2+.").position, 2U);
}

TEST(ExpressionTest, DivisionByZero) {
    const Error e = errorOf("1/(2-2)");
    EXPECT_EQ(e.code, ErrorCode::DivisionByZero);
    EXPECT_EQ(e.position, 1U);
}

// ---- 資源を使い果たす入力への対策 ----

TEST(ExpressionTest, MaxLengthIsAccepted) {
    // "1+1+...+1" をちょうど上限の長さにする
    std::string input = "1";
    while (input.size() + 2 <= kMaxExpressionLength) {
        input += "+1";
    }
    ASSERT_LE(input.size(), kMaxExpressionLength);
    EXPECT_GT(valueOf(input), 0.0);
}

TEST(ExpressionTest, TooLongInputIsRejected) {
    const Error e = errorOf(std::string(kMaxExpressionLength + 1, '1'));
    EXPECT_EQ(e.code, ErrorCode::ExpressionTooLong);
}

TEST(ExpressionTest, NestingUpToLimitIsAccepted) {
    const auto n = static_cast<std::size_t>(kMaxNestingDepth);
    EXPECT_DOUBLE_EQ(valueOf(std::string(n, '(') + "1" + std::string(n, ')')), 1.0);
    EXPECT_DOUBLE_EQ(valueOf(std::string(n, '-') + "1"), 1.0); // 64 回の符号反転
}

TEST(ExpressionTest, TooDeepParenthesesAreRejected) {
    const auto n = static_cast<std::size_t>(kMaxNestingDepth) + 1;
    const Error e = errorOf(std::string(n, '(') + "1" + std::string(n, ')'));
    EXPECT_EQ(e.code, ErrorCode::ExpressionTooDeep);
}

TEST(ExpressionTest, TooManyUnaryOperatorsAreRejected) {
    const auto n = static_cast<std::size_t>(kMaxNestingDepth) + 1;
    const Error e = errorOf(std::string(n, '-') + "1");
    EXPECT_EQ(e.code, ErrorCode::ExpressionTooDeep);
}

TEST(ExpressionTest, OverflowIsDetected) {
    // 長さの上限（256 文字）の範囲では double があふれる式は作れないため、
    // 上限を広げて検出処理そのものを確かめる（多層防御）
    std::string input = "1" + std::string(200, '0');
    input += "*" + input + "*" + input;
    const Error e = errorOf(input, 4096);
    EXPECT_EQ(e.code, ErrorCode::Overflow);
}

TEST(ExpressionTest, OverflowInAdditionIsDetected) {
    const std::string big = "1" + std::string(308, '0'); // 1e308
    const Error e = errorOf(big + "+" + big, 4096);
    EXPECT_EQ(e.code, ErrorCode::Overflow);
    EXPECT_EQ(e.position, big.size());
}
