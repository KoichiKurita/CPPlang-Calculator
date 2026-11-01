#pragma once
#include "CalculatorState.h"
#include "Expression.h"
#include "NumberFormat.h"
#include <cmath>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

// 計算履歴の1件（例: expression = "(1+2)*3", result = 9）
struct HistoryEntry {
    std::string expression;
    double result{0};
};

inline bool operator==(const HistoryEntry& a, const HistoryEntry& b) {
    return a.expression == b.expression && a.result == b.result;
}

// 保持する履歴の件数の上限（古いものから削除する）
inline constexpr std::size_t kMaxHistoryEntries = 100;
// 履歴の式の長さの上限。ボタン計算の式（"123 + 4" など）も式の入力の上限内に収まる
inline constexpr std::size_t kMaxHistoryExpressionLength = kMaxExpressionLength;

inline const char* operatorSymbol(Operator op) {
    switch (op) {
        case Operator::Add:      return "+";
        case Operator::Subtract: return "-";
        case Operator::Multiply: return "*";
        case Operator::Divide:   return "/";
    }
    return "?"; // 列挙型の範囲外の値（到達不能） GCOVR_EXCL_LINE
}

// ボタン計算の式を文字列にする（例: lhs=2, op=Add, rhs=3 → "2 + 3"）
inline std::string describeOperation(const PendingOperation& pending, double rhs) {
    return formatNumber(pending.lhs) + " " + operatorSymbol(pending.op) + " " + formatNumber(rhs);
}

// 履歴の1件が正しい値か。保存データは改ざんされうるため、読み込み時にこれで検証する
inline bool isValidHistoryEntry(const HistoryEntry& entry) {
    return !entry.expression.empty() && entry.expression.size() <= kMaxHistoryExpressionLength &&
           std::isfinite(entry.result);
}

// 計算履歴（新しいものが先頭）。Qt に依存しない
class History {
public:
    void add(HistoryEntry entry) {
        if (!isValidHistoryEntry(entry)) {
            return;
        }
        entries_.insert(entries_.begin(), std::move(entry));
        if (entries_.size() > kMaxHistoryEntries) {
            entries_.resize(kMaxHistoryEntries);
        }
    }

    void clear() { entries_.clear(); }

    const std::vector<HistoryEntry>& entries() const { return entries_; }

    // 保存データから復元する。不正な値は読み飛ばし、上限を超えた分は捨てる
    void restore(const std::vector<HistoryEntry>& loaded) {
        entries_.clear();
        for (const auto& entry : loaded) {
            if (entries_.size() >= kMaxHistoryEntries) {
                break;
            }
            if (isValidHistoryEntry(entry)) {
                entries_.push_back(entry);
            }
        }
    }

private:
    std::vector<HistoryEntry> entries_;
};
