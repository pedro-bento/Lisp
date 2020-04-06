char* readline(char* buffer, size_t buffer_size, const char* prompt)
{
  printf("%s", prompt);
  return fgets(buffer, buffer_size, stdin);
}

char* read_file(const char* path)
{
  FILE *file;
  char *buf;
  uint64_t len;

  file = fopen(path, "r");
  if(!file) return NULL;

  fseek(file, 0, SEEK_END);
  len = ftell(file);
  fseek(file, 0, SEEK_SET);

  buf = malloc(len + 1);
  if(!buf) return NULL;

  fread(buf, 1, len, file);
  buf[len] = '\0';
  fclose(file);

  return buf;
}

void load_file(Atom env, const char* path)
{
  char* text;

  printf("Loading %s\n", path);
  text = read_file(path);
  if(text)
  {
    const char* p = text;
    Atom expr;
    while(parse_expr(p, &p, &expr) == Error_OK)
    {
      Atom result;
      Error err = eval_expr(expr, env, &result);
      if(err){
        printf("Error in expression:\n\t");
        print_expr(expr);
        printf("\n");
      }else{
        print_expr(result);
        printf("\n");
      }
    }
    free(text);
  }
}

int main(int argc, char const *argv[])
{
  char* buffer = (char*)malloc(64 * sizeof(char));

  Atom env = env_create(nil);
  env_set(env, make_sym("CAR"), make_builtin(builtin_car));
  env_set(env, make_sym("CDR"), make_builtin(builtin_cdr));
  env_set(env, make_sym("CONS"), make_builtin(builtin_cons));
  env_set(env, make_sym("+"), make_builtin(builtin_add));
  env_set(env, make_sym("-"), make_builtin(builtin_subtract));
  env_set(env, make_sym("*"), make_builtin(builtin_multiply));
  env_set(env, make_sym("/"), make_builtin(builtin_divide));
  env_set(env, make_sym("="), make_builtin(builtin_numequal));
  env_set(env, make_sym("<"), make_builtin(builtin_numless));
  env_set(env, make_sym(">"), make_builtin(builtin_numgreater));
  env_set(env, make_sym("APPLY"), make_builtin(builtin_apply));
  env_set(env, make_sym("EQ?"), make_builtin(builtin_equal));
  env_set(env, make_sym("PAIR?"), make_builtin(builtin_pairp));

  load_file(env, "stdlib.lisp");

  while((buffer = readline(buffer, 64, "> ")) != NULL)
  {
    const char *p = buffer;
    Error err;
    Atom expr, result;

    err = parse_expr(p, &p, &expr);
    if(!err)
    {
      err = eval_expr(expr, env, &result);
    }

    switch(err)
    {
      case Error_OK :{
        print_expr(result);
        printf("\n");
      } break;

      case Error_Syntax :{
        printf("Syntax error\n");
      } break;

      case Error_Unbound :{
        printf("Symbol not bound\n");
      } break;

      case Error_Args :{
        printf("Wrong number of arguments\n");
      } break;

      case Error_Type :{
        printf("Wrong type\n");
      } break;
    }
  }

  printf("\n");
  free(buffer);

  return 0;
}
