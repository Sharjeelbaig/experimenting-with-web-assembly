# Calculator (C) — WASM experiments

Single-file C calculator with exported functions suitable for compiling to WebAssembly
and running natively.

Files:
- `calculator.c` — single-file calculator (exports + CLI REPL)

Build & run (native):

```bash
clang -O2 calculator.c -o calculator -lm
./calculator
```

Build for Emscripten (WASM + JS glue):

```bash
emcc calculator.c -O3 -s MODULARIZE=1 -s EXPORT_NAME='createCalculatorModule' -o calculator.js
# or simple HTML wrapper
emcc calculator.c -O3 -o calculator.html
```

Example usage in JavaScript (EMSCRIPTEN - modularized build):

```js
const Module = await createCalculatorModule();
const add = Module.cwrap('add', 'number', ['number', 'number']);
console.log(add(1.5, 2.25)); // 3.75

const evalStr = Module.cwrap('eval_str', 'number', ['string']);
console.log(evalStr("3+4")); // 7
```

Build for WASI (WASM only):

```bash
# Install once (Homebrew)
brew install wasi-libc llvm lld wasi-runtimes

# Compile
/opt/homebrew/opt/llvm/bin/clang --target=wasm32-unknown-wasip1 -O3 calculator.c -o calculator.wasm
```

Notes:
- Functions are annotated with `EMSCRIPTEN_KEEPALIVE` so Emscripten keeps them in the build.
- `eval_str` is a simple parser for expressions of the form `a op b`.

If you'd like, I can also:
- add more operators or parentheses support
- create a small JS frontend that loads the wasm
