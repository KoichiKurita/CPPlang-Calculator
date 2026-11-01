#include "ui/KeyInput.h"
#include <gtest/gtest.h>

TEST(KeyInputTest, DigitKeys) {
    for (char c = '0'; c <= '9'; ++c) {
        const auto input = inputFromChar(c);
        ASSERT_TRUE(input.has_value());
        EXPECT_EQ(input->type, InputType::Digit);
        EXPECT_EQ(input->value, c - '0');
    }
}

TEST(KeyInputTest, OperatorKeys) {
    EXPECT_EQ(inputFromChar('+')->op, Operator::Add);
    EXPECT_EQ(inputFromChar('-')->op, Operator::Subtract);
    EXPECT_EQ(inputFromChar('*')->op, Operator::Multiply);
    EXPECT_EQ(inputFromChar('/')->op, Operator::Divide);
    EXPECT_EQ(inputFromChar('+')->type, InputType::Operator);
}

TEST(KeyInputTest, DecimalPointAndEqualKeys) {
    EXPECT_EQ(inputFromChar('.')->type, InputType::DecimalPoint);
    EXPECT_EQ(inputFromChar('=')->type, InputType::Equal);
}

TEST(KeyInputTest, OtherKeysAreIgnored) {
    EXPECT_FALSE(inputFromChar('a').has_value());
    EXPECT_FALSE(inputFromChar(' ').has_value());
    EXPECT_FALSE(inputFromChar(',').has_value());
}
