import pyparsing as pp


def _doc_parser():
    # brief description on first line
    brief = pp.restOfLine("brief").add_parse_action(lambda t: t[0].strip())

    # macro | KEYWORD args
    macro_name = pp.Word(pp.alphas.upper() + "_")
    macro_args = pp.restOfLine.add_parse_action(lambda t: t[0].strip())

    macro_entry = (macro_name + macro_args).add_parse_action(lambda t: (t[0], t[1]))

    grammar = brief + pp.ZeroOrMore(macro_entry)("data")
    return grammar


def parse_doc(text: str) -> dict:
    grammar = _doc_parser()
    results = grammar.parse_string(text)
    return {"brief": results.brief, "data": list(results.data)}
