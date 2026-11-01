#pragma once
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

// 表示用の数値フォーマット（Qt 非依存の純粋関数としてテスト可能にする）
// 有効数字15桁で表示する。double が誤差なく表現できる10進桁数（15桁）に合わせている
inline std::string formatNumber(double value) {
    if (value == 0) {
        return "0"; // -0 を "0" と表示する
    }

    std::ostringstream oss;
    oss.imbue(std::locale::classic()); // 環境のロケールに依存せず "." を小数点にする
    oss << std::setprecision(15) << value;
    return oss.str();
}
