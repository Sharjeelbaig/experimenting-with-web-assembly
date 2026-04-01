/* calculator.c — Single-file calculator suitable for native and WASM builds.
 *
 * Exports:
 *  - add, subtract, multiply, divide, power
 *  - compute(a, b, op)      // op: '+', '-', '*', '/', '^'
 *  - eval_str("a op b")     // parses a simple expression string
 *
 * When building with Emscripten, these are kept/exported via EMSCRIPTEN_KEEPALIVE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

EMSCRIPTEN_KEEPALIVE double add(double a, double b) { return a + b; }
EMSCRIPTEN_KEEPALIVE double subtract(double a, double b) { return a - b; }
EMSCRIPTEN_KEEPALIVE double multiply(double a, double b) { return a * b; }
EMSCRIPTEN_KEEPALIVE double divide(double a, double b) { return (b == 0.0) ? NAN : (a / b); }
EMSCRIPTEN_KEEPALIVE double power(double a, double b) { return pow(a, b); }

EMSCRIPTEN_KEEPALIVE double compute(double a, double b, char op) {
    switch (op) {
        case '+': return add(a, b);
        case '-': return subtract(a, b);
        case '*': case 'x': case 'X': return multiply(a, b);
        case '/': return divide(a, b);
        case '^': return power(a, b);
        default:  return NAN;
    }
}

EMSCRIPTEN_KEEPALIVE double eval_str(const char *expr) {
    if (!expr) return NAN;
    char local[256];
    size_t len = strlen(expr);
    if (len >= sizeof(local)) return NAN;
    strncpy(local, expr, sizeof(local));
    local[sizeof(local)-1] = '\0';
    // trim leading whitespace
    char *s = local;
    while (*s == ' ' || *s == '\t') s++;
    double a = 0, b = 0;
    char op = 0;
    if (sscanf(s, "%lf %c %lf", &a, &op, &b) == 3) {
        return compute(a, b, op);
    }
    if (sscanf(s, "%lf%c%lf", &a, &op, &b) == 3) {
        return compute(a, b, op);
    }
    return NAN;
}

int main(void) {
    char buf[256];
    printf("Simple calculator REPL — enter expressions like: 2 + 3 or 4*5\n");
    printf("Supports + - * / ^  (type q to quit)\n");
    while (1) {
        printf("> ");
        if (!fgets(buf, sizeof(buf), stdin)) break;
        // trim leading whitespace
        char *s = buf;
        while (*s == ' ' || *s == '\t') s++;
        if (s[0] == 'q' || s[0] == 'Q') break;
        double result = eval_str(s);
        if (isnan(result)) {
            printf("Invalid expression\n");
        } else {
            printf("= %g\n", result);
        }
    }
    return 0;
}
