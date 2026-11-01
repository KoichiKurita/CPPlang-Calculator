#include "app/CalculatorController.h"
#include "ui/QtCalculatorUI.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[]) {
    const QApplication app(argc, argv);

    QtCalculatorUI ui;
    CalculatorController controller(ui);

    controller.run();
    ui.show();

    // --smoke-test: 起動して直後に終了する。配布物に必要なライブラリ・プラグインが
    // そろっていて、アプリが起動できることを CI で確かめるために使う
    if (QCoreApplication::arguments().contains(QStringLiteral("--smoke-test"))) {
        QTimer::singleShot(0, &app, &QCoreApplication::quit);
    }

    return QApplication::exec();
}
