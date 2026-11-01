#include "ui/NumberFormat.h"
#include <gtest/gtest.h>

TEST(NumberFormatTest, SmallIntegerIsShownAsIs) {
    EXPECT_EQ(formatNumber(5), "5");
}

TEST(NumberFormatTest, SevenDigitsAreNotExponential) {
    // QString::number の既定（6桁）では 1.23457e+06 になっていた
    EXPECT_EQ(formatNumber(1234567), "1234567");
}

TEST(NumberFormatTest, FifteenDigitsAreShownInFull) {
    EXPECT_EQ(formatNumber(999'999'999'999'999.0), "999999999999999");
}

TEST(NumberFormatTest, NegativeNumber) {
    EXPECT_EQ(formatNumber(-42), "-42");
}

TEST(NumberFormatTest, DecimalResult) {
    EXPECT_EQ(formatNumber(7.0 / 2.0), "3.5");
}

TEST(NumberFormatTest, NegativeZeroIsShownAsZero) {
    EXPECT_EQ(formatNumber(-0.0), "0");
}
