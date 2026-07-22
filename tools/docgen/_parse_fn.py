import pyparsing as pp


def _c_fn_parser():
    # gcc allows dollar sign in function name
    identifier = pp.Word(pp.alphas + "_" + "$", pp.alphanums + "_" + "$")
    single_space = pp.White().add_parse_action(pp.replace_with(" "))

    type_qualifiers = (
        pp.Keyword("const")
        | pp.Keyword("unsigned")
        | pp.Keyword("struct")
        | pp.Keyword("enum")
        | pp.Keyword("static")
        | pp.Keyword("inline")
    )
    pointers = pp.ZeroOrMore("*")

    type_expr = pp.Combine(
        pp.Optional(pp.OneOrMore(type_qualifiers + single_space))
        + identifier
        + pp.ZeroOrMore(single_space)
        + pointers
    )

    parameter = pp.Group(type_expr("type") + pp.Optional(identifier)("name"))

    # ignore void parameter
    param_list = pp.Keyword("void").suppress() | pp.DelimitedList(parameter)(
        "parameters"
    )

    c_function = (
        type_expr("return_type")
        + identifier("function_name")
        + pp.Suppress("(")
        + pp.Optional(param_list)
        + pp.Suppress(")")
        + pp.Optional(pp.Literal(";")).suppress()
    )
    c_function.ignore(pp.cStyleComment | pp.cppStyleComment)

    return c_function


def parse_c_signature(signature: str) -> dict:
    parser = _c_fn_parser()

    try:
        result = parser.parse_string(signature)
    except pp.exceptions.ParseException as e:
        return {}

    # Extract structured results
    data = {
        "return_type": result.return_type.strip(),
        "function_name": result.function_name,
        "arguments": [],
    }

    if "parameters" in result:
        for param in result.parameters:
            data["arguments"].append(
                {
                    "type": param.type.strip(),
                    "name": param.name if "name" in param else None,
                }
            )

    return data
