import pyparsing as pp

def _c_struct_parser():
    # gcc allows dollar sign in identifiers
    identifier = pp.Word(pp.alphas + "_" + "$", pp.alphanums + "_" + "$")
    single_space = pp.White().add_parse_action(pp.replace_with(" "))

    type_qualifiers = (
        pp.Keyword("const")
        | pp.Keyword("unsigned")
        | pp.Keyword("signed")
        | pp.Keyword("struct")
        | pp.Keyword("enum")
    )
    pointers = pp.Combine(pp.ZeroOrMore("*"))

    type_expr = pp.Combine(
        pp.Optional(pp.OneOrMore(type_qualifiers + single_space))
        + identifier
        + pp.ZeroOrMore(single_space)
        + pointers
    )

    # struct field := <type> <name>;
    field = pp.Group(type_expr("type") + identifier("name") + pp.Suppress(";"))

    struct_body = pp.Suppress("{") + pp.ZeroOrMore(field)("fields") + pp.Suppress("}")

    c_struct = (
        pp.Optional(pp.Keyword("typedef"))("is_typedef")
        + pp.Keyword("struct")
        + pp.Optional(identifier)("struct_tag")
        + struct_body
        + pp.Optional(identifier)("typedef_alias")
        + pp.Suppress(";")
    )
    c_struct.ignore(pp.cStyleComment | pp.cppStyleComment)

    return c_struct


def parse_c_struct(signature: str) -> dict:
    parser = _c_struct_parser()

    try:
        result = parser.parse_string(signature)
    except pp.exceptions.ParseException as e:
        return {}

    # Extract structured results
    data = {
        "name": result.typedef_alias or result.struct_tag,
        "fields": [],
    }

    for f in result.fields:
        data["fields"].append((f.name.strip(), f.type.strip()))

    return data
