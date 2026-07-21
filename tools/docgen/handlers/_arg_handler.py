def arg_handler(obj: object, arg: str) -> str:
    # arg_stmt := ARG name (description)
    arg = arg.strip()
    parts = arg.split(" ")

    name = arg.split(" ")[0]
    desc = ""
    arg_type = "<UNKNOWN>"

    arguments_list = obj.args
    for arg_obj in arguments_list:
        if arg_obj["name"] == name:
            arg_type = arg_obj["type"]
            break

    if len(parts) > 1:
        desc = " ".join(parts[1:])

    # properly end with period
    if not desc.endswith("."):
        desc += "."

    if obj.counters["ARG"] == 0:
        return f"""\
  Args:     {arg_type} {name}
              {desc}\
"""
    return f"""\
            {arg_type} {name}
              {desc}\
"""
