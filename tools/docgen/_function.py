from .handlers._arg_handler import arg_handler
from .handlers._returns_handler import returns_handler
from .handlers._requires_handler import requires_handler
from .handlers._note_handler import note_handler
from .handlers._modifies_handler import modifies_handler


_MAPPING = {
    "ARG": arg_handler,
    "RETURNS": returns_handler,
    "REQUIRES": requires_handler,
    "NOTE": note_handler,
    "MODIFIES": modifies_handler,
}


class Function(object):
    """Describes a function parsed"""

    def __init__(self, name: str, return_type: str, args: dict, doc: dict):
        self.name = name
        self.return_type = return_type
        self.args = args
        self.doc = doc

        self.brief = doc["brief"]

        self.counters: dict = dict.fromkeys(_MAPPING.keys(), 0)

    def generate_documentation(self) -> str:
        result = ""

        brief = self.brief
        data = self.doc["data"]

        header: str
        footer: str

        if self.name.startswith("_"):
            # private API function
            header = "F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            footer = "-----------------------------------------------------------------F-F"
        else:
            header = "F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F"
            footer = "F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F"

        result += header + "\n"

        result += f"  Function: {self.name}\n"
        result += "\n"
        result += f"  Summary:  {brief}\n"

        last_directive = "~"

        for doc_item, doc_value in data:
            if not doc_item in _MAPPING:
                print("E021 - unknown directive")
                print(f"E021 - {doc_item} is not known")
                exit(1)

            to_call = _MAPPING[doc_item]
            res = to_call(self, doc_value)

            # give larger separation between different blocks
            # note that using the tilde is illegal in acutal code
            if last_directive != "~" and last_directive != doc_item:
                result += "\n\n" + res
            else:
                result += "\n" + res
            last_directive = doc_item

            self.counters[doc_item] += 1

        result += "\n" + footer

        return result
