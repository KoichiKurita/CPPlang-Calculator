#pragma once
#include "CalculatorEvent.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

// 式の入力（例: "(1+2)*3"）を評価する。入力は信頼できないデータとして扱う
//
// 文法（EBNF）
//   expression = term , { ("+" | "-") , term } ;
//   term       = factor , { ("*" | "/") , factor } ;
//   factor     = ("+" | "-") , factor | primary ;
//   primary    = number | "(" , expression , ")" ;
//   number     = digits , [ "." , [ digits ] ] | "." , digits ;
//
// 資源を使い果たす入力への対策
//   - 入力の長さの上限（kMaxExpressionLength）
//   - 括弧・単項演算子の入れ子の深さの上限（kMaxNestingDepth）＝再帰によるスタックあふれを防ぐ
//   - 許可した文字（数字・小数点・+ - * / ・括弧・空白）以外は拒否する

inline constexpr std::size_t kMaxExpressionLength = 256;
inline constexpr int kMaxNestingDepth = 64;

namespace expression_detail {

enum class TokenKind : std::uint8_t {
    Number,
    Plus,
    Minus,
    Star,
    Slash,
    LeftParen,
    RightParen,
    End
};

struct Token {
    TokenKind kind{TokenKind::End};
    double value{0};
    std::size_t position{0};
};

inline Error syntaxError(std::size_t position) {
    return Error{ErrorCode::InvalidExpression, position};
}

inline bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

// 数値の文字列を double に変換する（環境のロケールに依存せず "." を小数点として扱う）
inline double parseNumber(std::string_view text) {
    std::istringstream iss{std::string(text)};
    iss.imbue(std::locale::classic());
    double value = 0;
    iss >> value;
    return value;
}

// 字句解析：入力を数値・演算子・括弧のトークン列に分ける（末尾に End を付ける）
inline std::variant<std::vector<Token>, Error> tokenize(std::string_view input) {
    std::vector<Token> tokens;
    std::size_t i = 0;
    while (i < input.size()) {
        const char c = input[i];
        if (c == ' ' || c == '\t') {
            ++i;
            continue;
        }
        if (isDigit(c) || c == '.') {
            const std::size_t start = i;
            int dots = 0;
            int digits = 0;
            while (i < input.size() && (isDigit(input[i]) || input[i] == '.')) {
                if (input[i] == '.') {
                    ++dots;
                } else {
                    ++digits;
                }
                ++i;
            }
            // "1.2.3" や "." のような数値は認めない
            if (dots > 1 || digits == 0) {
                return syntaxError(start);
            }
            const double value = parseNumber(input.substr(start, i - start));
            tokens.push_back({TokenKind::Number, value, start});
            continue;
        }

        TokenKind kind{};
        switch (c) {
            case '+': kind = TokenKind::Plus; break;
            case '-': kind = TokenKind::Minus; break;
            case '*': kind = TokenKind::Star; break;
            case '/': kind = TokenKind::Slash; break;
            case '(': kind = TokenKind::LeftParen; break;
            case ')': kind = TokenKind::RightParen; break;
            default:  return syntaxError(i); // 許可していない文字
        }
        tokens.push_back({kind, 0, i});
        ++i;
    }
    tokens.push_back({TokenKind::End, 0, input.size()});
    return tokens;
}

// 構文解析と評価を同時に行う再帰下降パーサー
// 最初に見つかったエラーを error_ に記録し、以降の処理は打ち切る
//
// 再帰下降パーサーは文法の再帰（括弧の中に式がある）をそのまま関数の再帰で表すため、
// misc-no-recursion の指摘は設計どおり。再帰の深さは kMaxNestingDepth で制限している
// NOLINTBEGIN(misc-no-recursion)
class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    std::variant<double, Error> parse() {
        const double value = expression(0);
        if (!error_ && peek().kind != TokenKind::End) {
            fail(syntaxError(peek().position)); // 例: "1+2)" の余分な ")"
        }
        if (error_) {
            return *error_;
        }
        return value;
    }

private:
    // expression = term , { ("+" | "-") , term } ;
    double expression(int depth) {
        double value = term(depth);
        while (!error_ && (peek().kind == TokenKind::Plus || peek().kind == TokenKind::Minus)) {
            const Token op = next();
            const double rhs = term(depth);
            value = op.kind == TokenKind::Plus ? value + rhs : value - rhs;
            checkFinite(value, op.position);
        }
        return value;
    }

    // term = factor , { ("*" | "/") , factor } ;
    double term(int depth) {
        double value = factor(depth);
        while (!error_ && (peek().kind == TokenKind::Star || peek().kind == TokenKind::Slash)) {
            const Token op = next();
            const double rhs = factor(depth);
            if (error_) {
                break;
            }
            if (op.kind == TokenKind::Slash) {
                if (rhs == 0) {
                    fail(Error{ErrorCode::DivisionByZero, op.position});
                    break;
                }
                value /= rhs;
            } else {
                value *= rhs;
            }
            checkFinite(value, op.position);
        }
        return value;
    }

    // factor = ("+" | "-") , factor | primary ;
    double factor(int depth) {
        // 括弧・単項演算子のたびに depth が増える。上限を超えたら再帰を止める
        if (depth > kMaxNestingDepth) {
            fail(Error{ErrorCode::ExpressionTooDeep, peek().position});
            return 0;
        }
        if (peek().kind == TokenKind::Plus) {
            next();
            return factor(depth + 1);
        }
        if (peek().kind == TokenKind::Minus) {
            next();
            return -factor(depth + 1);
        }
        return primary(depth);
    }

    // primary = number | "(" , expression , ")" ;
    double primary(int depth) {
        const Token token = next();
        if (token.kind == TokenKind::Number) {
            return token.value;
        }
        if (token.kind == TokenKind::LeftParen) {
            const double value = expression(depth + 1);
            if (error_) {
                return 0;
            }
            if (peek().kind != TokenKind::RightParen) {
                fail(syntaxError(peek().position)); // 例: "(1+2" の閉じ括弧の不足
                return 0;
            }
            next();
            return value;
        }
        fail(syntaxError(token.position)); // 例: "1+" や "*2"
        return 0;
    }

    // トークン列は必ず End で終わり、index_ は End を越えて進まない
    const Token& peek() const { return tokens_[index_]; }

    Token next() {
        const Token token = tokens_[index_];
        if (token.kind != TokenKind::End) {
            ++index_;
        }
        return token;
    }

    void fail(const Error& error) {
        if (!error_) {
            error_ = error;
        }
    }

    void checkFinite(double value, std::size_t position) {
        if (!std::isfinite(value)) {
            fail(Error{ErrorCode::Overflow, position});
        }
    }

    std::vector<Token> tokens_;
    std::size_t index_{0};
    std::optional<Error> error_;
};
// NOLINTEND(misc-no-recursion)

} // namespace expression_detail

// 式を評価する。成功すれば値、失敗すればエラー（種類と位置）を返す
// maxLength はテストで上限を変えるための引数（通常は既定値を使う）
inline std::variant<double, Error>
evaluateExpression(std::string_view input, std::size_t maxLength = kMaxExpressionLength) {
    if (input.size() > maxLength) {
        return Error{ErrorCode::ExpressionTooLong, maxLength};
    }
    auto tokens = expression_detail::tokenize(input);
    if (const auto* error = std::get_if<Error>(&tokens)) {
        return *error;
    }
    return expression_detail::Parser(
               std::get<std::vector<expression_detail::Token>>(std::move(tokens)))
        .parse();
}
