# Changelog

このプロジェクトの主な変更履歴です。
形式は [Keep a Changelog](https://keepachangelog.com/ja/1.1.0/) に、
バージョニングは [Semantic Versioning](https://semver.org/lang/ja/) に従います。

## [Unreleased]
### Added
- 小数点（.）・符号反転（±）・バックスペース（⌫）
- キーボード入力（0-9 . + - * / Enter(=) Esc(C) Backspace）。キーの変換は Qt 非依存の `inputFromChar` に分離
- 小数・符号反転・バックスペース・キーボード変換のユニットテスト

### Changed
- 入力中の数値を文字列（`entry`）で保持し、"0." や "1.50" を入力どおりに表示するよう変更（`enteringNumber` を置き換え）
- ボタン配置を一般的な電卓の配置（C ⌫ ± / ・ 7 8 9 * ・ … ・ 0 . =）に変更

## [1.3.0] - 2026-11-01
### Changed
  - GitHub Actions を最新のメジャーバージョンに更新（Dependabot：checkout v7・upload-artifact v7・download-artifact v8・action-gh-release v3）
  - `.h` ファイルを GitHub の言語統計で C++ として扱うよう設定

## [1.2.0] - 2026-11-01
### Added
- clang-format / clang-tidy / cppcheck による静的解析と CI ジョブ（static-analysis）
- SECURITY.md（脆弱性の報告方法）と Dependabot による GitHub Actions の自動更新
- 実行ファイルのハードニング（スタック保護・_FORTIFY_SOURCE・ASLR/DEP・PIE/Full RELRO）と CI での検証
- gcovr によるカバレッジ計測（目標: 行 95% / 分岐 90%）、CI のジョブサマリー表示、GitHub Pages へのレポートとバッジの公開
- 未テストだった引き算・割り算・不正な演算子状態・コントローラのクリアのテスト

### Changed
- 状態を型安全に再設計：`hasOp` / `op` / `stored` を `std::optional<PendingOperation>` に統合し、演算子を `enum class Operator` に変更（矛盾した状態を表現できなくした）
- エラーを `const char*` から `enum class ErrorCode` に変更し、表示文言への変換を UI 層（`errorMessage`）に分離
- エラーをモーダルダイアログ（QMessageBox）ではなく表示欄に表示するよう変更
- GitHub Actions をコミット SHA で固定し、checkout で認証情報を保持しないよう変更
- 設計図を PlantUML + PNG から Mermaid（README に直接記述）に移行し、v1.1.0 の実装に合わせて更新
- clang-format によるコードの一括整形
- clang-tidy の指摘に基づく改善（ラムダのキャプチャの明示、インターフェースのコピー/ムーブ禁止、enum の基底型指定など）
- CI の clang-format を PyPI の 23.1.3 に固定し、ローカル（MSYS2 の 23.x）と整形結果が一致するよう変更

## [1.1.0] - 2026-10-28
### Added
- PR・Issue テンプレート、CHANGELOG、.editorconfig
- 改行コードを LF に統一する .gitattributes 設定
- GoogleTest によるドメイン層のユニットテスト
- Qt なしでドメイン層をビルド・テストできる `CALC_BUILD_GUI` オプション
- LICENSE（MIT）

### Changed
- CI を刷新：Linux（Qt なし・ASan/UBSan）と Windows（GUI）の2ジョブ構成、最小権限化、同一ブランチの重複実行をキャンセル
- README を現在の構成に合わせて更新（テスト・品質/セキュリティ・配布・開発フローを追記）
- CI ワークフローのファイル名を windows.yml から ci.yml に変更

### Fixed
- CI の MSYS2 実行環境が UCRT64 になっておらずビルドできなかった問題
- `CalculatorController::run()` 終了後に破棄済みのローカル変数を参照していた未定義動作（ダングリング参照）
- 演算子を連続で押すと計算が実行されていた問題（演算子を置き換えるよう修正）
- 連続計算中の0除算エラーが表示されなかった問題
- 入力桁数の上限がなく inf になっていた問題。入力値の検証とオーバーフロー検出を追加
- 7桁以上の数値が指数表記（例: 1.23457e+06）で表示されていた問題
- テストが失敗しても CI が成功扱いになっていた問題
- Release の exe が Qt 未導入の PC で起動しなかった問題（Qt/MinGW の DLL を同梱した zip と SHA256 を公開）

## [1.0.0] - 2026-10-27
### Added
- 四則演算・連続計算・クリア機能を持つ Qt6 電卓
- 純粋関数によるドメイン層とレイヤードアーキテクチャ
- GitHub Actions による Windows ビルドと Release 作成

[Unreleased]: https://github.com/KoichiKurita/CPPlang-Calculator/compare/v1.3.0...HEAD
[1.3.0]: https://github.com/KoichiKurita/CPPlang-Calculator/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/KoichiKurita/CPPlang-Calculator/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/KoichiKurita/CPPlang-Calculator/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/KoichiKurita/CPPlang-Calculator/releases/tag/v1.0.0
