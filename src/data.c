typedef enum
{
  Error_OK = 0,
  Error_Syntax,
  Error_Unbound,
  Error_Args,
  Error_Type,
} Error;

typedef struct Atom Atom;
typedef struct Pair Pair;

typedef Error (*Builtin)(Atom args, Atom *result);

struct Atom
{
  enum
  {
    AtomType_Nil,
    AtomType_Pair,
    AtomType_Symbol,
    AtomType_Integer,
    AtomType_Builtin,
    AtomType_Closure,
    AtomType_Macro,
  } type;

  union
  {
    Pair* pair;
    const char* symbol;
    int64_t integer;
    Builtin builtin;
  } value;

};

struct Pair
{
  Atom atom[2];
};

// for historical reasons the elements of a Pair are called car and cdr
#define car(p) ((p).value.pair->atom[0])
#define cdr(p) ((p).value.pair->atom[1])

// check if is Nil
#define nilp(atom) ((atom).type == AtomType_Nil)
// global Nil
static const Atom nil = { AtomType_Nil };

// Symbol Table to keep track of all symbols created
static Atom sym_table = { AtomType_Nil };

// Integers and (pointers to) Strings can be copied, but pairs need to be allocated
// cons allocates a pair and assigns its two elements
Atom cons(Atom car_val, Atom cdr_val)
{
  Atom p;

  p.type = AtomType_Pair;
  p.value.pair = (Pair*)malloc(sizeof(Pair));
  if(!p.value.pair) fprintf(stderr, "Failed to allocate atom value.pair\n");

  car(p) = car_val;
  cdr(p) = cdr_val;

  return p;
}

bool listp(Atom expr)
{
  while(!nilp(expr))
  {
    if(expr.type != AtomType_Pair)
      return false;
    expr = cdr(expr);
  }
  return true;
}

Atom make_int(int64_t x)
{
  return (Atom)
  {
    .type = AtomType_Integer,
    .value.integer = x,
  };
}

Atom make_sym(const char* s)
{
  Atom a, p;

  // check if sym already exists
  p = sym_table;
  while(!nilp(p))
  {
    a = car(p);
    if(strcmp(a.value.symbol, s) == 0)
    {
      return a;
    }
    p = cdr(p);
  }

  // sym does not exist
  a.type = AtomType_Symbol;
  a.value.symbol = strdup(s);
  sym_table = cons(a, sym_table);

  return a;
}

Atom make_builtin(Builtin fn)
{
  return (Atom)
  {
    .type = AtomType_Builtin,
    .value.builtin = fn,
  };
}

Error make_closure(Atom env, Atom args, Atom body, Atom *resul)
{
  Atom p;

  if(!listp(body))
    return Error_Syntax;

  // check if args are all symbols
  p = args;
  while(!nilp(p))
  {
    if(p.type == AtomType_Symbol){
      break;
    }else if(p.type != AtomType_Pair || car(p).type != AtomType_Symbol){
      return Error_Type;
    }
    p = cdr(p);
  }

  *resul = cons(env, cons(args, body));
  resul->type = AtomType_Closure;

  return Error_OK;
}

void print_expr(Atom atom)
{
  switch(atom.type)
  {
    case AtomType_Nil :{
      printf("NILL");
    } break;

    case AtomType_Pair :{
      printf("(");
      print_expr(car(atom));
      atom = cdr(atom);
      while(!nilp(atom)){
        if(atom.type == AtomType_Pair){
          printf(" ");
          print_expr(car(atom));
          atom = cdr(atom);
        }else{
          printf(" . ");
          print_expr(atom);
          break;
        }
      }
      printf(")");
    } break;

    case AtomType_Symbol :{
      printf("%s", atom.value.symbol);
    } break;

    case AtomType_Integer :{
      printf("%ld", atom.value.integer);
    } break;

    case AtomType_Builtin :{
      printf("#<BUILTIN:%p>", atom.value.builtin);
    } break;

    case AtomType_Closure :{
      printf("CLOSURE");
    } break;

    case AtomType_Macro :{
      printf("MACRO");
    } break;
  }
}

Atom copy_list(Atom list)
{
  Atom a, p;

  if(nilp(list))
    return nil;

  a = cons(car(list), nil);
  p = a;
  list = cdr(list);

  while(!nilp(list))
  {
    cdr(p) = cons(car(list), nil);
    p = cdr(p);
    list = cdr(list);
  }

  return a;
}
