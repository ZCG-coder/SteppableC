def fld_handler(obj: object, arg: str):
    args = arg.split(" ")
    name = args[0]
    desc = ""
    if len(args) > 1:
        desc = " ".join(args[1:])
    if not desc.endswith("."):
        desc += "."

    field_type = "<UNKNOWN>"
    for field in obj.fields:
        if field[0] == name:
            field_type = field[1]

    if obj.counters["FLD"] == 0:
        return f"""\
  Fields:   {field_type} {name}
              {desc}\
"""
    return f"""\
            {field_type} {name}
              {desc}\
"""
