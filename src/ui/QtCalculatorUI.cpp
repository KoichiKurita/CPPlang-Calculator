#include "QtCalculatorUI.h"
#include "../domain/Expression.h"
#include "../domain/NumberFormat.h"
#include "ErrorMessage.h"
#include "KeyInput.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

QtCalculatorUI::QtCalculatorUI()
    : expressionInput_(new QLineEdit), display_(new QLineEdit("0")), historyList_(new QListWidget) {
    // 式の入力欄。Enter で評価し、結果を表示欄に出す
    expressionInput_->setObjectName("expression"); // GUI テストから参照するための名前
    expressionInput_->setPlaceholderText("Expression, e.g. (1+2)*3  [Enter]");
    expressionInput_->setMaxLength(static_cast<int>(kMaxExpressionLength));
    connect(expressionInput_, &QLineEdit::returnPressed, this, [this] {
        Input input{InputType::Expression};
        input.expression = expressionInput_->text().toStdString();
        emitInput(input);
    });

    // 表示部（キーボード入力をウィジェット本体で受けるため、フォーカスを取らせない）
    display_->setReadOnly(true);
    display_->setAlignment(Qt::AlignRight);
    display_->setMinimumHeight(30);
    display_->setFocusPolicy(Qt::NoFocus);
    display_->setObjectName("display"); // GUI テストから参照するための名前

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(expressionInput_);
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

    // 計算履歴（右側）。クリックするとその結果を続けて計算に使える
    historyList_->setObjectName("history"); // GUI テストから参照するための名前
    historyList_->setFixedWidth(220);
    // フォーカスを取らせない（リストがキー入力を奪うとキーボードで計算できなくなるため）
    historyList_->setFocusPolicy(Qt::NoFocus);
    connect(historyList_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        emitInput({InputType::UseHistory, historyList_->row(item)});
    });

    auto* clearHistory = new QPushButton("Clear history");
    clearHistory->setFocusPolicy(Qt::NoFocus);
    connect(clearHistory, &QPushButton::clicked, this,
            [this] { emitInput({InputType::ClearHistory}); });

    auto* historyLayout = new QVBoxLayout;
    historyLayout->addWidget(new QLabel("History"));
    historyLayout->addWidget(historyList_);
    historyLayout->addWidget(clearHistory);

    auto* rootLayout = new QHBoxLayout;
    rootLayout->addLayout(mainLayout);
    rootLayout->addLayout(historyLayout);
    setLayout(rootLayout);
    setWindowTitle("QtCalculator");
    // ウィンドウの大きさを中身に合わせて固定する（固定値だと部品の間に余白ができるため）
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    rootLayout->setSizeConstraint(QLayout::SetFixedSize);

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
                display_->setText(QString::fromStdString(errorText(e)));
            }
        },
        event);
}

void QtCalculatorUI::showHistory(const std::vector<HistoryEntry>& entries) {
    historyList_->clear();
    for (const auto& entry : entries) {
        historyList_->addItem(
            QString::fromStdString(entry.expression + " = " + formatNumber(entry.result)));
    }
}

void QtCalculatorUI::keyPressEvent(QKeyEvent* event) {
    // 式の入力欄で処理されなかったキー（Enter・Esc など）は親のこのウィジェットに届く。
    // そのまま電卓の操作として扱うと「Enter で = が押される」ことになるため区別する
    if (expressionInput_->hasFocus()) {
        if (event->key() == Qt::Key_Escape) {
            // Esc：入力欄を空にして、ボタン電卓のキーボード操作に戻る
            expressionInput_->clear();
            setFocus();
        }
        return;
    }

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
