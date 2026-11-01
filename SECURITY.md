# Security Policy

## Supported Versions

Only the latest release receives security fixes.

| Version | Supported |
| ------- | --------- |
| 1.3.x   | ✅        |
| < 1.3   | ❌        |

## Reporting a Vulnerability

Please **do not report security vulnerabilities through public GitHub issues**.

Use GitHub's private vulnerability reporting instead:

1. Open the [Security tab](https://github.com/KoichiKurita/CPPlang-Calculator/security) of this repository
2. Click **Report a vulnerability**
3. Describe the issue, steps to reproduce, and the affected version

You can expect an initial response within **7 days**.
If the report is confirmed, a fix will be released as a patch version and the reporter will be credited (unless anonymity is requested).

## Security Measures in This Project

- Input validation in the domain layer (digits, operators, digit limit, overflow detection)
- Tests run with AddressSanitizer / UndefinedBehaviorSanitizer in CI
- Static analysis with clang-tidy and cppcheck in CI
- Hardened binaries (stack protector, `_FORTIFY_SOURCE`, ASLR, DEP / PIE, Full RELRO)
- GitHub Actions pinned by commit SHA and kept up to date by Dependabot
- Release packages published with SHA256 checksums
