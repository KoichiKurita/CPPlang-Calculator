#include "ui/QtHistoryStore.h"
#include <QSettings>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include <limits>

namespace {

class QtHistoryStoreTest : public ::testing::Test {
protected:
    QString path() const { return dir_.filePath("history.ini"); }

    QTemporaryDir dir_; // テストごとに一時ディレクトリを作り、終了時に削除する
};

} // namespace

TEST_F(QtHistoryStoreTest, MissingFileLoadsNothing) {
    QtHistoryStore store(path());

    EXPECT_TRUE(store.load().empty());
}

TEST_F(QtHistoryStoreTest, SavedEntriesCanBeLoaded) {
    const std::vector<HistoryEntry> entries{{"(1+2)*3", 9}, {"0.1 + 0.2", 0.1 + 0.2}};
    QtHistoryStore(path()).save(entries);

    // 別のインスタンス（＝アプリの再起動）で読み込める
    EXPECT_EQ(QtHistoryStore(path()).load(), entries);
}

TEST_F(QtHistoryStoreTest, SaveReplacesPreviousEntries) {
    QtHistoryStore store(path());
    store.save({{"1 + 1", 2}, {"2 + 2", 4}});
    store.save({{"3 + 3", 6}});

    EXPECT_EQ(QtHistoryStore(path()).load(), (std::vector<HistoryEntry>{{"3 + 3", 6}}));
}

TEST_F(QtHistoryStoreTest, NonNumericResultsAreSkipped) {
    // 保存ファイルを直接書き換えて、不正な値を入れる
    {
        QSettings settings(path(), QSettings::IniFormat);
        settings.beginWriteArray("history", 2);
        settings.setArrayIndex(0);
        settings.setValue("expression", "tampered");
        settings.setValue("result", "not a number");
        settings.setArrayIndex(1);
        settings.setValue("expression", "1 + 1");
        settings.setValue("result", 2.0);
        settings.endArray();
    }

    EXPECT_EQ(QtHistoryStore(path()).load(), (std::vector<HistoryEntry>{{"1 + 1", 2}}));
}

TEST_F(QtHistoryStoreTest, HugeSizeIsNotTrusted) {
    // 件数（size）だけを極端に大きく書き換えても、上限までしか読まない
    {
        QSettings settings(path(), QSettings::IniFormat);
        settings.beginWriteArray("history", 1);
        settings.setArrayIndex(0);
        settings.setValue("expression", "1 + 1");
        settings.setValue("result", 2.0);
        settings.endArray();
        settings.setValue("history/size", 2000000000);
    }

    EXPECT_LE(QtHistoryStore(path()).load().size(), kMaxHistoryEntries);
}
