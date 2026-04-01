const createCalculator = require("./calculator.js");

(async () => {
  const calc = await createCalculator();

  // Wrap exports cleanly
  const add      = calc.cwrap("add",      "number", ["number", "number"]);
  const subtract = calc.cwrap("subtract", "number", ["number", "number"]);
  const multiply = calc.cwrap("multiply", "number", ["number", "number"]);
  const divide   = calc.cwrap("divide",   "number", ["number", "number"]);
  const power    = calc.cwrap("power",    "number", ["number", "number"]);
  const compute  = calc.cwrap("compute",  "number", ["number", "number", "number"]);

  // eval_str needs special handling — C strings aren't JS strings
  function evalStr(expr) {
    return calc.ccall(
      "eval_str",          // C function name
      "number",            // return type
      ["string"],          // arg types
      [expr]               // args — Emscripten handles UTF8 encoding for you
    );
  }

  // Use them
  console.log(add(2, 3));           // 5
  console.log(divide(10, 0));       // NaN
  console.log(power(2, 8));         // 256
  console.log(compute(10, 5, 43));  // 43 = '+'.charCodeAt(0) → 15
  console.log(evalStr("12 * 4"));   // 48
  console.log(evalStr("2 ^ 10"));   // 1024
})();