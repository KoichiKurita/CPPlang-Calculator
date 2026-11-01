#pragma once
#include "../domain/History.h"
#include <vector>

// 履歴の保存先のインターフェース。保存の方法（QSettings など）をコントローラから切り離す
class IHistoryStore {
public:
    IHistoryStore() = default;
    virtual ~IHistoryStore() = default;

    IHistoryStore(const IHistoryStore&) = delete;
    IHistoryStore& operator=(const IHistoryStore&) = delete;
    IHistoryStore(IHistoryStore&&) = delete;
    IHistoryStore& operator=(IHistoryStore&&) = delete;

    // 保存されている履歴（新しいものが先頭）。読み込んだ値の検証は History::restore で行う
    virtual std::vector<HistoryEntry> load() = 0;
    virtual void save(const std::vector<HistoryEntry>& entries) = 0;
};

// 何も保存しない実装（Null Object）。保存先を指定しないときに使う
class NullHistoryStore : public IHistoryStore {
public:
    std::vector<HistoryEntry> load() override { return {}; }
    void save(const std::vector<HistoryEntry>& /*entries*/) override {}
};
