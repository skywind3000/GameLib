# /api-audit — API 变更文档审计

When user changes GameLib public API, or says "check docs" / "API changed" / "sync docs", run this audit.

## Steps

1. **Read both header files** — Read `GameLib.h` and `GameLib.SDL.h` to extract the `GameLib` class public methods.
2. **Compare** — Identify differences between Win32 and SDL public APIs. Flag:
   - Methods present in one but missing in the other
   - Signature mismatches (different param types, return types)
   - Newly added or removed methods
3. **Check docs** — Read these docs and verify they reflect the current API state:
   - `docs/manual.md` — Public API docs, must match both lines
   - `docs/quickref.md` — Quick API reference, must be up to date
   - `docs/GameLib.md` — Internal design, update if implementation changed
   - `docs/GameLib.SDL.md` — SDL-specific spec, update if SDL API changed
4. **Report** — Output a clear table of:
   - What changed in the API
   - Which docs are out of sync
   - Specific line/file references for each mismatch

## Rules

- Do NOT auto-update docs. Present the diff to user and let them confirm.
- Always check BOTH lines. Changes to GameLib.h must be considered for GameLib.SDL.h parity.
- Flag any breaking changes or semantic shifts that should be noted.
- If no mismatches found, report "All docs in sync, no action needed."
