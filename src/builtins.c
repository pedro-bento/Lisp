Error builtin_car(Atom args, Atom *result)
{
  if(nilp(args) || !nilp(cdr(args)))
    return Error_Args;

  if(nilp(car(args))){
    *result = nil;
  }else if(car(args).type != AtomType_Pair){
    return Error_Type;
  }else{
    *result = car(car(args));
  }

  return Error_OK;
}

Error builtin_cdr(Atom args, Atom *result)
{
  if(nilp(args) || !nilp(cdr(args)))
    return Error_Args;

  if(nilp(car(args))){
    *result = nil;
  }else if(car(args).type != AtomType_Pair){
    return Error_Type;
  }else{
    *result = cdr(car(args));
  }

  return Error_OK;
}

Error builtin_cons(Atom args, Atom *result)
{
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  *result = cons(car(args), car(cdr(args)));

  return Error_OK;
}

Error builtin_add(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = make_int(a.value.integer + b.value.integer);

  return Error_OK;
}

Error builtin_subtract(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = make_int(a.value.integer - b.value.integer);

  return Error_OK;
}

Error builtin_multiply(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = make_int(a.value.integer * b.value.integer);

  return Error_OK;
}

Error builtin_divide(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = make_int(a.value.integer / b.value.integer);

  return Error_OK;
}

Error builtin_numequal(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = (a.value.integer == b.value.integer) ? make_sym("True") : nil;

  return Error_OK;
}

Error builtin_numless(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = (a.value.integer < b.value.integer) ? make_sym("True") : nil;

  return Error_OK;
}

Error builtin_numgreater(Atom args, Atom *result)
{
  Atom a, b;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type != AtomType_Integer || b.type != AtomType_Integer)
    return Error_Type;

  *result = (a.value.integer > b.value.integer) ? make_sym("True") : nil;

  return Error_OK;
}

Error builtin_apply(Atom args, Atom *result)
{
  Atom fn;
  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  fn = car(args);
  args = car(cdr(args));

  if(!listp(args))
    return Error_Syntax;

  return apply(fn, args, result);
}

Error builtin_equal(Atom args, Atom *result)
{
  Atom a, b;
  bool equal;

  if(nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if(a.type == b.type){
    switch(a.type)
    {
      case AtomType_Nil :{
        equal = true;
      } break;

      case AtomType_Pair :
      case AtomType_Closure :
      case AtomType_Macro :{
        equal = (a.value.pair == b.value.pair);
      } break;

      case AtomType_Symbol :{
        equal = (a.value.symbol == b.value.symbol);
      } break;

      case AtomType_Integer :{
        equal = (a.value.integer == b.value.integer);
      } break;

      case AtomType_Builtin :{
        equal = (a.value.builtin == b.value.builtin);
      } break;

    }
  }else{
    equal = false;
  }

  *result = equal ? make_sym("True") : nil;

  return Error_OK;
}

Error builtin_pairp(Atom args, Atom *result)
{
  if(nilp(args) || !nilp(cdr(args)))
    return Error_Args;
  *result = (car(args).type == AtomType_Pair) ? make_sym("True") : nil;
  return Error_OK;
}
