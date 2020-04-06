// finds the start and end of the next token
Error lex(const char *str, const char **start, const char **end)
{
  const char *ws = " \t\n";
  const char *delim = "() \t\n";
  const char *prefix = "()\'`";

  // remove whitespaces
  str += strspn(str, ws);

  if(str[0] == '\0')
  {
    *start = *end = NULL;
    return Error_Syntax;
  }

  *start = str;

  if(strchr(prefix, str[0]) != NULL){
    *end = str + 1;
  }else if(str[0] == ','){
    *end = str + (str[1] == '@' ? 2 : 1);
  }else{
    *end = str + strcspn(str, delim);
  }

  return Error_OK;
}

Error parse_expr(const char *input, const char **end, Atom *result);

// parse simple data i.e. Integers, Symbols...
Error parse_simple(const char *start, const char *end, Atom *result)
{
  char *buf, *p;

  // Integer
  int64_t val = strtoll(start, &p, 10);
  if(p == end)
  {
    result->type = AtomType_Integer;
    result->value.integer = val;
    return Error_OK;
  }

  // Nil or Symbol
  buf = (char*)malloc(end - start + 1);
  p = buf;
  while(start != end)
  {
    // converting to upper case because it's the traditional behaviour
    *p++ = toupper(*start);
    ++start;
  }
  *p = '\0';

  if(strcmp(buf, "NIL") == 0){
    *result = nil;
  }else{
    *result = make_sym(buf);
  }

  free(buf);

  return Error_OK;
}

// parse improper list and pairs
Error parse_list(const char *start, const char **end, Atom *result)
{
  Atom p;

  *end = start;
  p = *result = nil;

  while(true)
  {
    const char *token;
    Atom item;
    Error err;

    err = lex(*end, &token, end);
    if(err)
    {
      return err;
    }

    if(token[0] == ')')
    {
      return Error_OK;
    }

    // improper list
    if(token[0] == '.' && *end - token == 1)
    {
      if(nilp(p))
      {
        return Error_Syntax;
      }

      err = parse_expr(*end, end, &item);
      if(err)
      {
        return err;
      }

      cdr(p) = item;

      // get to closing ')'
      err = lex(*end, &token, end);
      if(!err && token[0] != ')')
      {
        err = Error_Syntax;
      }

      return err;
    }

    err = parse_expr(token, end, &item);
    if(err)
    {
      return err;
    }

    if(nilp(p)){
      // First item
      *result = cons(item, nil);
      p = *result;
    }else{
      cdr(p) = cons(item, nil);
      p = cdr(p);
    }
  }
}

Error parse_expr(const char *input, const char **end, Atom *result)
{
  const char *token;
  Error err;

  err = lex(input, &token, end);
  if(err)
  {
    return err;
  }

  if(token[0] == '('){
    return parse_list(*end, end, result);
  }else if(token[0] == ')'){
    return Error_Syntax;
  }else if(token[0] == '\''){
    *result = cons(make_sym("QUOTE"), cons(nil, nil));
    return parse_expr(*end, end, &car(cdr(*result)));
  }else if(token[0] == '`'){
    *result = cons(make_sym("QUASIQUOTE"), cons(nil, nil));
    return parse_expr(*end, end, &car(cdr(*result)));
  }else if(token[0] == ','){
    *result = cons(make_sym(
      token[1] == '@' ? "UNQUOTE-SPLICING" : "UNQUOTE"),
      cons(nil, nil));
    return parse_expr(*end, end, &car(cdr(*result)));
  }else{
    return parse_simple(token, *end, result);
  }
}
