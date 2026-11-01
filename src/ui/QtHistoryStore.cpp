#include "QtHistoryStore.h"
#include <algorithm>

namespace {
// 静的な QString は初期化時に例外を投げうる（cert-err58-cpp）ため、文字列リテラルの定数にする
constexpr const char* kArrayKey = "history";
constexpr const char* kExpressionKey = "expression";
constexpr const char* kResultKey = "result";
} // namespace

QtHistoryStore::QtHistoryStore() : settings_(std::make_unique<QSettings>()) {}

QtHistoryStore::QtHistoryStore(const QString& iniFilePath)
    : settings_(std::make_unique<QSettings>(iniFilePath, QSettings::IniFormat)) {}

std::vector<HistoryEntry> QtHistoryStore::load() {
    std::vector<HistoryEntry> entries;
    // 保存データは改ざんされうるため、件数（size）を信用せず上限までしか読まない
    const int size = settings_->beginReadArray(kArrayKey);
    const int count = std::min(size, static_cast<int>(kMaxHistoryEntries));
    for (int i = 0; i < count; ++i) {
        settings_->setArrayIndex(i);
        bool ok = false;
        const double result = settings_->value(kResultKey).toDouble(&ok);
        const QString expression = settings_->value(kExpressionKey).toString();
        // 数値として読めない値は読み飛ばす（式の長さや非有限値の検証は History::restore で行う）
        if (ok) {
            entries.push_back(HistoryEntry{expression.toStdString(), result});
        }
    }
    settings_->endArray();
    return entries;
}

void QtHistoryStore::save(const std::vector<HistoryEntry>& entries) {
    settings_->remove(kArrayKey);
    settings_->beginWriteArray(kArrayKey, static_cast<int>(entries.size()));
    for (std::size_t i = 0; i < entries.size(); ++i) {
        settings_->setArrayIndex(static_cast<int>(i));
        settings_->setValue(kExpressionKey, QString::fromStdString(entries[i].expression));
        settings_->setValue(kResultKey, entries[i].result);
    }
    settings_->endArray();
    settings_->sync(); // アプリが異常終了しても失われないよう、すぐに書き込む
}
