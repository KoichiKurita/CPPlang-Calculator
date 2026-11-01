#include "domain/History.h"
#include <gtest/gtest.h>
#include <limits>
#include <string>
#include <vector>

TEST(HistoryTest, NewestEntryComesFirst) {
    History history;
    history.add({"1 + 1", 2});
    history.add({"2 * 3", 6});

    ASSERT_EQ(history.entries().size(), 2U);
    EXPECT_EQ(history.entries()[0].expression, "2 * 3");
    EXPECT_EQ(history.entries()[1].expression, "1 + 1");
}

TEST(HistoryTest, OldestEntriesAreDroppedOverCapacity) {
    History history;
    for (std::size_t i = 0; i < kMaxHistoryEntries + 5; ++i) {
        history.add({std::to_string(i), static_cast<double>(i)});
    }

    ASSERT_EQ(history.entries().size(), kMaxHistoryEntries);
    EXPECT_EQ(history.entries().front().expression, std::to_string(kMaxHistoryEntries + 4));
    EXPECT_EQ(history.entries().back().expression, "5");
}

TEST(HistoryTest, InvalidEntriesAreRejected) {
    History history;
    history.add({"", 1});
    history.add({std::string(kMaxHistoryExpressionLength + 1, '1'), 1});
    history.add({"0/0", std::numeric_limits<double>::quiet_NaN()});
    history.add({"big", std::numeric_limits<double>::infinity()});

    EXPECT_TRUE(history.entries().empty());
}

TEST(HistoryTest, Clear) {
    History history;
    history.add({"1 + 1", 2});
    history.clear();

    EXPECT_TRUE(history.entries().empty());
}

TEST(HistoryTest, RestoreSkipsInvalidEntriesAndCapsTheCount) {
    // 改ざんされた保存データを想定：不正な値が混ざり、件数も上限を超えている
    std::vector<HistoryEntry> loaded;
    loaded.push_back({"", 1});
    loaded.push_back({"bad", std::numeric_limits<double>::infinity()});
    for (std::size_t i = 0; i < kMaxHistoryEntries + 10; ++i) {
        loaded.push_back({"1 + " + std::to_string(i), 1.0 + static_cast<double>(i)});
    }

    History history;
    history.restore(loaded);

    ASSERT_EQ(history.entries().size(), kMaxHistoryEntries);
    EXPECT_EQ(history.entries().front().expression, "1 + 0");
}

TEST(HistoryTest, DescribeOperation) {
    EXPECT_EQ(describeOperation({2, Operator::Add}, 3), "2 + 3");
    EXPECT_EQ(describeOperation({12.5, Operator::Multiply}, -2), "12.5 * -2");
    EXPECT_EQ(describeOperation({9, Operator::Subtract}, 4), "9 - 4");
    EXPECT_EQ(describeOperation({8, Operator::Divide}, 2), "8 / 2");
}
