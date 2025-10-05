```markdown

## Overview
This repository contains three small C++ projects implemented
Projects:
- **Polynomial ADT** — insertion, display, addition, multiplication, differentiation.
- **Text Editor** — cursor-based editing with insert/delete/move and `|` cursor visualization.
- **UNO Simulation** — simplified deterministic 2–4 player UNO (no wilds), RNG seed `1234` for reproducible runs.

All code targets `g++ -std=c++17`.

## Public repository


```

[https://github.com/](https://github.com/)<your-username>/<your-repo-name>

````

## Approach
- Keep header edits **minimal** (only add `virtual`). No data members were added to headers.
- Implementations live entirely in `.cpp` files. Two safe patterns were used:
  1. When the header is a minimal interface, implement a `Concrete*` subclass inside the `.cpp` and expose a factory (e.g. `createPolynomial()`) that returns `std::unique_ptr<...>`.
  2. When the header requires value semantics, store per-instance state inside the `.cpp` keyed by the object pointer (`this`) in a `std::unordered_map`. Implement copy/move/destructors in the `.cpp` so objects copied or returned by value preserve their state.
- Prefer simple, readable code (arrays, `std::deque`, `std::vector`) suitable for student assignments.
- UNO uses `std::mt19937` seeded with `1234` to make shuffles deterministic for testing.

## Files (per project)

### Polynomial
- `Polynomial.h`
- `polynomial.cpp`
- `main.cpp`

### Text Editor
- `TextEditor.h`
- `TextEditor.cpp`
- `main.cpp`

### UNO
- `UNO.h`
- `UNO.cpp`
- `main.cpp`

## Build & Run
From the project folder (or from the subfolder for the project you want):

Polynomial:
```bash
g++ -std=c++17 -Wall main.cpp polynomial.cpp -o poly.exe
./poly.exe       # or .\poly.exe on Windows PowerShell
````

Text Editor:

```bash
g++ -std=c++17 -Wall main.cpp TextEditor.cpp -o editor.exe
./editor.exe
```

UNO:

```bash
g++ -std=c++17 -Wall main.cpp UNO.cpp -o uno.exe
./uno.exe
```

If you see strange linker errors on Windows, delete stale artifacts (`del *.o,*.obj,*.exe` or `rm -f *.o *.obj *.exe`) then recompile.

## Sample Output

Polynomial example:

```
p1.toString(): 3x^4 + 2x^2 - x + 5
sum.toString(): 4x^4 + 2x^2 - x + 6
prod.toString(): 8x^5 + 4x^3 - 2x^2 + 12x
deriv.toString(): 12x^3 + 4x - 1
```

Text Editor example:

```
After insert 'a': a|
After insert 'b': ab|
After move left: a|b
After insert 'c': ac|b
After delete: a|
After move left twice: |a
After move right twice: a|
```

UNO example (deterministic with seed 1234):

```
Player 0's turn, Direction: Clockwise, Top: Blue 3, Players cards: P0:7, P1:7
Player 1's turn, Direction: Clockwise, Top: Blue 8, Players cards: P0:7, P1:6
...
```

## Challenges faced

* **Header file**: Initially I thought I could implement the sample output with the header file given without changing it even a bit but that wasnt possible 
* **Header constraint**: Only adding `virtual` made it impossible to add data members to headers. Two practical workarounds were used: factory + concrete subclass, or hidden per-instance storage in the `.cpp`.
* **C++ special-member rules**: Implicit/explicit constructors, and object copying required careful handling to avoid linker errors and lost state.
* **UNO specifics**: Handling Reverse with 2 players, deck exhaustion (reshuffle discard except top), Draw Two effects, and stalemate detection required explicit rules and testing.
* **Testing**: Ensuring identical shuffles across runs mandated seeding the RNG (`1234`).

## License

Use for coursework. Add a LICENSE file if you plan public distribution.

```
```
