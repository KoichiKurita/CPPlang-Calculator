#include "app/CalculatorController.h"
#include "ui/QtCalculatorUI.h"
#include "ui/QtHistoryStore.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[]) {
    const QApplication app(argc, argv);
    // QSettings の保存場所は組織名・アプリ名で決まる（例: Linux は
    // ~/.config/KoichiKurita/QtCalculator.conf）
    QCoreApplication::setOrganizationName(QStringLiteral("KoichiKurita"));
    QCoreApplication::setApplicationName(QStringLiteral("QtCalculator"));

    QtCalculatorUI ui;
    QtHistoryStore historyStore;
    CalculatorController controller(ui, historyStore);

    controller.run();
    ui.show();

    // --smoke-test: 起動して直後に終了する。配布物に必要なライブラリ・プラグインが
    // そろっていて、アプリが起動できることを CI で確かめるために使う
    if (QCoreApplication::arguments().contains(QStringLiteral("--smoke-test"))) {
        QTimer::singleShot(0, &app, &QCoreApplication::quit);
    }

    return QApplication::exec();
}
