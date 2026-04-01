# Calculator (C) — WebAssembly + Node.js

Single-file C calculator with exported functions suitable for compiling to WebAssembly
and running natively or in Node.js.

## Files

- `calculator.c` — single-file calculator (exports + CLI REPL)
- `calculator.js` — Emscripten-generated JavaScript glue code
- `calculator.wasm` — compiled WebAssembly binary
- `index.js` — Node.js example using the compiled WASM

## Build & Run (Native)

```bash
clang -O2 calculator.c -o calculator -lm
./calculator
```

## Build for Emscripten (WASM + JS glue for Node.js)

```bash
emcc calculator.c -o calculator.js \
  -s EXPORTED_FUNCTIONS='["_add","_subtract","_multiply","_divide","_power","_compute","_eval_str"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","stringToUTF8","allocate","ALLOC_NORMAL"]' \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="createCalculator" \
  -lm
```

### Emscripten Build Options Explained

- **`EXPORTED_FUNCTIONS`** — List of C functions to export (prefix with `_`)
- **`EXPORTED_RUNTIME_METHODS`** — Emscripten runtime helpers needed for calling C functions
  - `ccall` — Call C function directly
  - `cwrap` — Create JavaScript wrapper for C function
  - `stringToUTF8` — Convert JS strings to C strings
  - `allocate` — Allocate memory for passing data to C
- **`MODULARIZE=1`** — Creates a module factory function instead of global
- **`EXPORT_NAME`** — Name of the factory function
- **`-lm`** — Link the math library (for pow, etc.)

## Usage in Node.js

```js
const createCalculator = require('./calculator.js');

createCalculator().then(Module => {
  // Method 1: Use cwrap to create JavaScript functions
  const add = Module.cwrap('add', 'number', ['number', 'number']);
  const subtract = Module.cwrap('subtract', 'number', ['number', 'number']);
  const multiply = Module.cwrap('multiply', 'number', ['number', 'number']);
  const divide = Module.cwrap('divide', 'number', ['number', 'number']);
  const power = Module.cwrap('power', 'number', ['number', 'number']);
  
  console.log('5 + 3 =', add(5, 3));           // 8
  console.log('10 - 4 =', subtract(10, 4));    // 6
  console.log('7 * 6 =', multiply(7, 6));      // 42
  console.log('20 / 4 =', divide(20, 4));      // 5
  console.log('2 ^ 8 =', power(2, 8));         // 256
  
  // Method 2: Use ccall to call directly
  const result = Module.ccall('add', 'number', ['number', 'number'], [15, 25]);
  console.log('15 + 25 =', result);            // 40
  
  // Method 3: Use eval_str for expression parsing
  const evalStr = Module.cwrap('eval_str', 'number', ['string']);
  console.log('eval("3+4") =', evalStr("3+4")); // 7
  console.log('eval("10*5") =', evalStr("10*5")); // 50
});
```

## Build for WASI (Standalone WASM)

```bash
# Install once (Homebrew)
brew install wasi-libc llvm lld wasi-runtimes

# Compile
/opt/homebrew/opt/llvm/bin/clang --target=wasm32-unknown-wasip1 -O3 calculator.c -o calculator.wasm
```

## Workflow: C Functions → WebAssembly → Node.js

1. **Write C functions** in `calculator.c`
2. **Annotate with `EMSCRIPTEN_KEEPALIVE`** to prevent dead-code elimination
3. **Compile with emcc** using the command above
4. **Load in Node.js** using the generated module
5. **Wrap functions** with `cwrap` for easy calling

## Notes

- Functions must be annotated with `EMSCRIPTEN_KEEPALIVE` or explicitly exported
- Function names in `EXPORTED_FUNCTIONS` need the `_` prefix
- `eval_str` is a simple parser for expressions of the form `a op b`
- Math operations use C's standard library (linked with `-lm`)
