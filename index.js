const fs = require("node:fs");
const path = require("node:path");
const { WASI } = require("node:wasi");

const wasmPath = path.join(__dirname, "calculator.wasm");
const wasmBuffer = fs.readFileSync(wasmPath);

(async () => {
  const wasi = new WASI({
    version: "preview1",
    args: process.argv,
    env: process.env,
    preopens: {
      "/": process.cwd()
    }
  });

  const module = await WebAssembly.compile(wasmBuffer);
  const instance = await WebAssembly.instantiate(module, {
    wasi_snapshot_preview1: wasi.wasiImport
  });

  const exports = instance.exports;

  // If built as a WASI command module, run _start.
  if (typeof exports._start === "function") {
    wasi.start(instance);
    return;
  }

  // If built as a reactor/library module, initialize and call exports.
  if (typeof exports._initialize === "function") {
    wasi.initialize(instance);
  }

  if (typeof exports.add === "function") {
    console.log(exports.add(2, 3));
  }
  if (typeof exports.compute === "function") {
    console.log(exports.compute(10, 5, "+".charCodeAt(0)));
  }
})().catch((err) => {
  console.error(err);
  process.exitCode = 1;
});