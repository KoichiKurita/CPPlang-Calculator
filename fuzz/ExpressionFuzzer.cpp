// libFuzzer による式の評価器のファジング
// ランダムな入力を大量に与え、クラッシュ・サニタイザーのエラー・不変条件の違反がないことを確かめる
#include "domain/Expression.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    const std::string_view input(reinterpret_cast<const char*>(data), size);
    const auto result = evaluateExpression(input);

    // 不変条件：成功したなら結果は有限の値、失敗したなら位置は入力の範囲内
    if (const auto* value = std::get_if<double>(&result)) {
        if (!std::isfinite(*value)) {
            std::abort();
        }
    } else if (std::get<Error>(result).position > kMaxExpressionLength) {
        std::abort();
    }
    return 0;
}
