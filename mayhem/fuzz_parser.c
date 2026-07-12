// libFuzzer harness for acwj's 02_Parser: drives the same scan -> binexpr ->
// interpretAST path as 02_Parser/main.c, but in-process over an in-memory file.
// The original Mayhem target was the raw file-input CLI (/acwj/02_Parser/parser @@);
// with halting sanitizers every malformed input exits immediately, so the CLI form
// is unproductive — this harness fuzzes the identical code path in-process instead
// (target name `parser` is preserved). The parser's error paths call exit(), which
// build.sh renames to acwj_exit (-Dexit=acwj_exit) so they longjmp back here.
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>

#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

static jmp_buf fuzz_env;

// Bound for exit() calls inside the parser (renamed via -Dexit=acwj_exit).
void acwj_exit(int code) {
  (void)code;
  longjmp(fuzz_env, 1);
}

// 02_Parser is an allocate-and-exit batch tool: AST nodes are malloc'd and never
// freed (main.c relies on process exit), and error paths longjmp past any cleanup.
const char *__asan_default_options(void) { return "detect_leaks=0"; }

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  FILE *f = fmemopen((void *)data, size, "r");
  if (f == NULL)
    return 0;
  Infile = f;
  Line = 1;
  Putback = '\n';
  Token.token = 0;
  Token.intvalue = 0;
  if (setjmp(fuzz_env) == 0) {
    scan(&Token);
    struct ASTnode *n = binexpr();
    interpretAST(n);
  }
  fclose(f);
  Infile = NULL;
  return 0;
}
