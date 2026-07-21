def returns_handler(obj: object, args: str):
    return_type = obj.return_type
    return f"""\
  Returns:  {return_type}
              {args}"""
