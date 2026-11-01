# libFuzzer のビルド（clang が必要）。AddressSanitizer / UBSan と組み合わせて実行する
add_executable(ExpressionFuzzer
    ExpressionFuzzer.cpp
)
target_link_libraries(ExpressionFuzzer PRIVATE domain)
target_compile_options(ExpressionFuzzer PRIVATE
    -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
target_link_options(ExpressionFuzzer PRIVATE -fsanitize=fuzzer,address,undefined)
