Atom env_create(Atom parent)
{
  return cons(parent, nil);
}

Error env_get(Atom env, Atom symbol, Atom *result)
{
  Atom parent = car(env);
  Atom bs = cdr(env);

  while(!nilp(bs))
  {
    Atom b = car(bs);
    if(car(b).value.symbol == symbol.value.symbol)
    {
      *result = cdr(b);
      return Error_OK;
    }
    bs = cdr(bs);
  }

  if(nilp(parent))
  {
    return Error_Unbound;
  }

  return env_get(parent, symbol, result);
}

Error env_set(Atom env, Atom symbol, Atom value)
{
  Atom bs = cdr(env);
  Atom b = nil;

  while(!nilp(bs))
  {
    b = car(bs);
    if(car(b).value.symbol == symbol.value.symbol)
    {
      cdr(b) = value;
      return Error_OK;
    }
    bs = cdr(bs);
  }

  b = cons(symbol, value);
  cdr(env) = cons(b, cdr(env));

  return Error_OK;
}

Error eval_expr(Atom expr, Atom env, Atom *result);
Error apply(Atom fn, Atom args, Atom *result)
{
  Atom env, arg_names, body;

  if(fn.type == AtomType_Builtin)
    return (*fn.value.builtin)(args, result);
  else if(fn.type != AtomType_Closure)
    return Error_Type;

  env = env_create(car(fn));
  arg_names = car(cdr(fn));
  body = cdr(cdr(fn));

  // bind the arguments
  while(!nilp(arg_names))
  {
    if(arg_names.type == AtomType_Symbol)
    {
      env_set(env, arg_names, args);
      args = nil;
      break;
    }

    if(nilp(args))
      return Error_Args;
    env_set(env, car(arg_names), car(args));
    arg_names = cdr(arg_names);
    args = cdr(args);
  }

  if(!nilp(args))
    return Error_Args;

  // evaluate the body
  while(!nilp(body))
  {
    Error err = eval_expr(car(body), env, result);
    if(err) return err;
    body = cdr(body);
  }

  return Error_OK;
}

Error eval_expr(Atom expr, Atom env, Atom *result)
{
  Atom op, args, p;
  Error err;

  if(expr.type == AtomType_Symbol){
      return env_get(env, expr, result);
  }else if(expr.type != AtomType_Pair){
    *result = expr;
    return Error_OK;
  }

  if(!listp(expr))
  {
    return Error_Syntax;
  }

  op = car(expr);
  args = cdr(expr);

  if(op.type == AtomType_Symbol)
  {
    if(strcmp(op.value.symbol, "QUOTE") == 0){
      // needs to be exactly 1
      if(nilp(args) || !nilp(cdr(args)))
        return Error_Args;

      *result = car(args);
      return Error_OK;

    }else if(strcmp(op.value.symbol, "DEFINE") == 0){
      Atom sym, val;

      // needs to be exactly 2
      if(nilp(args) || nilp(cdr(args)))
        return Error_Args;

      sym = car(args);
      if(sym.type == AtomType_Pair){
        err = make_closure(env, cdr(sym), cdr(args), &val);
        sym = car(sym);
        if(sym.type != AtomType_Symbol)
          return Error_Type;
      }else if(sym.type == AtomType_Symbol){
        if(!nilp(cdr(cdr(args))))
          return Error_Args;
        err = eval_expr(car(cdr(args)), env, &val);
      }else{
        return Error_Type;
      }

      if(err) return err;

      *result = sym;
      return env_set(env, sym, val);
    }else if(strcmp(op.value.symbol, "LAMBDA") == 0){
      if(nilp(args) || nilp(cdr(args)))
        return Error_Args;
      return make_closure(env, car(args), cdr(args), result);
    }else if(strcmp(op.value.symbol, "IF") == 0){
      Atom cond, val;

      if(nilp(args) || nilp(cdr(args)) || nilp(cdr(cdr(args))) || !nilp(cdr(cdr(cdr(args)))))
        return Error_Args;

      err = eval_expr(car(args), env, &cond);
      if(err) return err;

      val = nilp(cond) ? car(cdr(cdr(args))) : car(cdr(args));
      return eval_expr(val, env, result);
    }else if(strcmp(op.value.symbol, "DEFMACRO") == 0){
      Atom name, macro;
      Error err;

      if(nilp(args) || nilp(cdr(args)))
        return Error_Args;

      if(car(args).type != AtomType_Pair)
        return Error_Syntax;

      name = car(car(args));
      if(name.type != AtomType_Symbol)
        return Error_Type;

      err = make_closure(env, cdr(car(args)), cdr(args), &macro);
      if(err) return err;

      macro.type = AtomType_Macro;
      *result = name;
      return env_set(env, name, macro);
    }
  }

  // evaluate operator
  err = eval_expr(op, env, &op);
  if(err) return err;

  // is it a macro?
  if(op.type == AtomType_Macro)
  {
    Atom expansion;
		op.type = AtomType_Closure;
		err = apply(op, args, &expansion);
		if (err)
			return err;
		return eval_expr(expansion, env, result);
  }

  // evaluate arguments
  args = copy_list(args); // copy to not overwrite the original
  p = args;
  while(!nilp(p))
  {
    err = eval_expr(car(p), env, &car(p));
    if(err) return err;
    p = cdr(p);
  }

  return apply(op, args, result);
}
