def requires_handler(obj: object, arg: str):
    if obj.counters["REQUIRES"] != 0:
        return f"""\
            * {arg}\
"""
    return f"""\
  Requires: * {arg}\
"""
