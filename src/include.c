// were are using a single compilation unit
// for more information visit:
// https://en.wikipedia.org/wiki/Single_Compilation_Unit
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// sources
// make sure you include them by order of dependency
// i.e. if A depends on B include A first then B
#include "data.c"
#include "parser.c"
#include "expressions.c"
#include "builtins.c"
#include "lisp.c"
