#pragma once
#include "../app/IHistoryStore.h"
#include <QSettings>
#include <QString>
#include <memory>
#include <vector>

// QSettings による履歴の保存先
// 保存場所は OS ごとに異なる（Windows: レジストリ、macOS: plist、Linux: ~/.config の ini ファイル）
class QtHistoryStore : public IHistoryStore {
public:
    // OS 標準の保存先を使う（QCoreApplication の組織名・アプリ名で場所が決まる）
    QtHistoryStore();
    // 指定した ini ファイルを使う（テスト用）
    explicit QtHistoryStore(const QString& iniFilePath);

    std::vector<HistoryEntry> load() override;
    void save(const std::vector<HistoryEntry>& entries) override;

private:
    std::unique_ptr<QSettings> settings_;
};
