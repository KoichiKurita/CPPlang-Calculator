#include "QtCalculatorUI.h"
#include "ErrorMessage.h"
#include "KeyInput.h"
#include "NumberFormat.h"

#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QVBoxLayout>

QtCalculatorUI::QtCalculatorUI() {
    // 表示部（キーボード入力をウィジェット本体で受けるため、フォーカスを取らせない）
    display_ = new QLineEdit("0");
    display_->setReadOnly(true);
    display_->setAlignment(Qt::AlignRight);
    display_->setMinimumHeight(30);
    display_->setFocusPolicy(Qt::NoFocus);

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(display_);

    auto* grid = new QGridLayout;

    // ボタンを1つ作って配置する。ボタンもフォーカスを取らない（Space キーで誤って押されるのを防ぐ）
    auto addButton = [&](const QString& text, const Input& input, int row, int col,
                         int colSpan = 1) {
        auto* btn = new QPushButton(text);
        btn->setFocusPolicy(Qt::NoFocus);
        connect(btn, &QPushButton::clicked, this, [this, input] { emitInput(input); });
        grid->addWidget(btn, row, col, 1, colSpan);
    };

    // 配置：
    //   C  ⌫  ±  /
    //   7  8  9  *
    //   4  5  6  -
    //   1  2  3  +
    //   0     .  =
    addButton("C", {InputType::Clear}, 0, 0);
    addButton("⌫", {InputType::Backspace}, 0, 1);
    addButton("±", {InputType::ToggleSign}, 0, 2);
    addButton("/", {InputType::Operator, 0, Operator::Divide}, 0, 3);

    for (int digit = 1; digit <= 9; ++digit) {
        const int row = 3 - ((digit - 1) / 3);
        const int col = (digit - 1) % 3;
        addButton(QString::number(digit), {InputType::Digit, digit}, row, col);
    }
    addButton("*", {InputType::Operator, 0, Operator::Multiply}, 1, 3);
    addButton("-", {InputType::Operator, 0, Operator::Subtract}, 2, 3);
    addButton("+", {InputType::Operator, 0, Operator::Add}, 3, 3);

    addButton("0", {InputType::Digit, 0}, 4, 0, 2);
    addButton(".", {InputType::DecimalPoint}, 4, 2);
    addButton("=", {InputType::Equal}, 4, 3);

    mainLayout->addLayout(grid);
    setLayout(mainLayout);
    setWindowTitle("QtCalculator");
    setFixedSize(300, 300);

    // キーボード入力を受け付ける
    setFocusPolicy(Qt::StrongFocus);
}

void QtCalculatorUI::onInput(std::function<void(Input)> handler) {
    handler_ = std::move(handler);
}

void QtCalculatorUI::handleEvent(const CalculatorEvent& event) {
    std::visit(
        [&](auto&& e) {
            using T = std::decay_t<decltype(e)>;

            if constexpr (std::is_same_v<T, DisplayUpdated>) {
                // 入力中は入力した文字列をそのまま表示する（"0." や "1.50" を保つため）
                const std::string text = e.entry.empty() ? formatNumber(e.value) : e.entry;
                display_->setText(QString::fromStdString(text));
            } else if constexpr (std::is_same_v<T, Error>) {
                // モーダルダイアログは使わず表示欄に出す（次の入力で表示が更新される）
                display_->setText(QString::fromUtf8(errorMessage(e.code)));
            }
        },
        event);
}

void QtCalculatorUI::keyPressEvent(QKeyEvent* event) {
    // 文字を持たないキー
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:     emitInput({InputType::Equal}); return;
        case Qt::Key_Escape:    emitInput({InputType::Clear}); return;
        case Qt::Key_Backspace: emitInput({InputType::Backspace}); return;
        default:                break;
    }

    // 文字を持つキー（数字・小数点・演算子）
    const QString text = event->text();
    if (text.size() == 1) {
        if (const auto input = inputFromChar(text.at(0).toLatin1())) {
            emitInput(*input);
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

void QtCalculatorUI::emitInput(const Input& input) {
    if (handler_) {
        handler_(input);
    }
}
