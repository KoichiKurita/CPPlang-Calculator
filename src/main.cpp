#include "app/CalculatorController.h"
#include "ui/QtCalculatorUI.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    const QApplication app(argc, argv);

    QtCalculatorUI ui;
    CalculatorController controller(ui);

    controller.run();
    ui.show();

    return QApplication::exec();
}
