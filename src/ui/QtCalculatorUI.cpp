#include "QtCalculatorUI.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

QtCalculatorUI::QtCalculatorUI() {
    // 表示部
    display_ = new QLineEdit("0");
    display_->setReadOnly(true);
    display_->setAlignment(Qt::AlignRight);
    display_->setMinimumHeight(30);

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(display_);

    auto* grid = new QGridLayout;

    // 数字ボタン（1〜9）
    int number = 1;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            auto* btn = new QPushButton(QString::number(number));
            connect(btn, &QPushButton::clicked, this,
                    [=] { emitDigit(number); });
            grid->addWidget(btn, r, c);
            ++number;
        }
    }

    // 0 ボタン
    auto* zero = new QPushButton("0");
    connect(zero, &QPushButton::clicked, this,
            [=] { emitDigit(0); });
    grid->addWidget(zero, 3, 1);

    // = ボタン
    auto* equal = new QPushButton("=");
    connect(equal, &QPushButton::clicked, this,
            [=] { emitEqual(); });
    grid->addWidget(equal, 3, 2);

    // 演算子ボタン
    auto addOp = [&](const QString& text, char op, int r, int c) {
        auto* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this,
                [=] { emitOp(op); });
        grid->addWidget(btn, r, c);
    };

    addOp("+", '+', 0, 3);
    addOp("-", '-', 1, 3);
    addOp("*", '*', 2, 3);
    addOp("/", '/', 3, 3);

    mainLayout->addLayout(grid);

    // C（クリア）ボタン
    auto* clear = new QPushButton("C");
    connect(clear, &QPushButton::clicked, this,
            [=] {
                if (handler_) {
                    handler_({InputType::Clear});
                }
            });

    mainLayout->addWidget(clear);

    setLayout(mainLayout);
    setWindowTitle("QtCalculator");
    setFixedSize(300, 260);
}

void QtCalculatorUI::onInput(std::function<void(Input)> handler) {
    handler_ = std::move(handler);
}

void QtCalculatorUI::handleEvent(const CalculatorEvent& event) {
    std::visit([&](auto&& e) {
        using T = std::decay_t<decltype(e)>;

        if constexpr (std::is_same_v<T, DisplayUpdated>) {
            display_->setText(QString::number(e.value));
        }
        else if constexpr (std::is_same_v<T, Error>) {
            QMessageBox::warning(this, "Error", e.message);
        }
    }, event);
}

void QtCalculatorUI::emitDigit(int d) {
    if (handler_) {
        handler_({InputType::Digit, d});
    }
}

void QtCalculatorUI::emitOp(char op) {
    if (handler_) {
        handler_({InputType::Operator, 0, op});
    }
}

void QtCalculatorUI::emitEqual() {
    if (handler_) {
        handler_({InputType::Equal});
    }
}
