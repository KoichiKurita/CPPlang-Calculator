TEST(CalculatorLogicTest, Addition) {
    CalculatorState s{0, {}, 0, false};

    auto [s1, _] = inputDigit(s, 2);
    auto [s2, _] = inputOperator(s1, '+');
    auto [s3, _] = inputDigit(s2, 3);
    auto [s4, e] = calculate(s3);

    auto display = std::get<DisplayUpdated>(e);
    EXPECT_EQ(display.value, 5);
}