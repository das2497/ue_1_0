# Phase 0 — FPS foundation

Compile the C++ module in UE **4.25**, then PIE.

If a Third Person Blueprint is still the level pawn, set **World Settings → GameMode Override** to `tp_1_0GameMode` (default pawn is the C++ FPS character).

## Controls

| Key | Action |
|-----|--------|
| WASD | Move |
| Mouse | Look |
| Left Shift | Sprint (standing only) |
| Left Ctrl | Crouch toggle (from prone → crouch) |
| Z or C | Prone toggle (from prone → crouch) |
| Space | Jump (crouch → stand; blocked while prone) |

## Health

- 100 HP, **no medkits**
- After **4s** without damage, regen **12 HP/s**
- Death: input off, level restarts after **2s** (stub)

## Not in this phase

Guns, AI, binoculars, map computer, mission, HUD.
