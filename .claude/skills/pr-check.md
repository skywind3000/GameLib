# /pr-check — PR 提交前约束检查

Before user commits or pushes, run this constraint check.

## Checks

### 1. C++11 Compliance (GameLib.h)
- Grep for C++14+ features in modified files:
  - `auto` return type deduction (e.g., `auto func() ->`)
  - `std::make_unique`
  - Structured bindings (`auto [a, b]`)
  - `if constexpr`
  - `std::optional`, `std::variant`, `std::any`
  - `[[maybe_unused]]`, `[[nodiscard]]` attributes
- Only GameLib.h must comply. GameLib.SDL.h uses C++11 baseline but is less strict.

### 2. No Cross-Line Mixing
- Check that no `.cpp` file includes both `GameLib.h` AND `GameLib.SDL.h`.
- Verify each line's constraints are kept independent.

### 3. No Direct Windows API Linking
- In GameLib.h: verify all Windows API calls go through `LoadLibrary`/`GetProcAddress`, not direct linking.
- Grep for `#pragma comment(lib` — should NOT exist.

### 4. No External Dependencies
- GameLib.h must not `#include` anything outside standard library and Windows SDK.
- No third-party library references.

### 5. Documentation Sync
- If public API changed, check `docs/manual.md` and `docs/quickref.md` are updated.
- If internal implementation changed, check `docs/GameLib.md` is updated.
- If SDL-specific behavior changed, check `docs/GameLib.SDL.md` is updated.
- If AGENTS.md behavior changed, verify it reflects current workflow.

### 6. No Build Artifacts
- Stage only source files. No `.exe`, `.o`, `.a` files.
- Use `git status` to verify staged files are clean.

## Output

Report as a checklist with PASS/FAIL for each check. If any FAIL, explain what specifically needs fixing.

## Rules

- Do NOT auto-fix. Report findings and let user decide.
- Be specific: cite file paths and line numbers for violations.
- If everything passes, say "All checks passed. Safe to commit."
