#include "ui/ErrorMessage.h"
#include <gtest/gtest.h>
#include <string>

TEST(ErrorMessageTest, EachErrorCodeHasMessage) {
    EXPECT_EQ(std::string(errorMessage(ErrorCode::NoOperation)), "No operation");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::DivisionByZero)), "Cannot divide by zero");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::Overflow)), "Overflow");
    EXPECT_EQ(std::string(errorMessage(ErrorCode::InvalidDigit)), "Invalid input");
}
