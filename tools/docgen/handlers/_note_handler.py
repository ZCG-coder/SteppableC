def note_handler(obj: object, arg: str):
    current_counter = obj.counters["NOTE"]

    if current_counter != 0:
        return f"""\
            {current_counter + 1}. {arg}\
"""
    return f"""\
  Note:     {current_counter + 1}. {arg}\
"""
