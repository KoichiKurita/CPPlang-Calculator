#include "ui/ErrorMessage.h"
#include <gtest/gtest.h>
#include <string>

TEST(ErrorMessageTest, EachErrorCodeHasMessage) {
    EXPECT_EQ(std::string(errorMessage(ErrorCode::NoOperation)), "No operation");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::DivisionByZero)), "Cannot divide by zero");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::Overflow)), "Overflow");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::InvalidDigit)), "Invalid input");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::InvalidExpression)), "Syntax error");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::ExpressionTooLong)), "Expression too long");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::ExpressionTooDeep)),
              "Expression too deeply nested");
}

TEST(ErrorMessageTest, SyntaxErrorShowsOneBasedPosition) {
    EXPECT_EQ(errorText(Error{ErrorCode::InvalidExpression, 4}), "Syntax error at 5");
}

TEST(ErrorMessageTest, OtherErrorsHaveNoPosition) {
    EXPECT_EQ(errorText(Error{ErrorCode::DivisionByZero, 1}), "Cannot divide by zero");
}
