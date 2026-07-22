from .handlers._note_handler import note_handler
from .handlers._fld_handler import fld_handler

_MAPPING = {
    "NOTE": note_handler,
    "FLD": fld_handler,
}

class Struct(object):
    """Describes a function parsed"""

    def __init__(self, name: str, fields: dict, doc: dict):
        self.name = name
        self.doc = doc
        self.fields = fields

        self.brief = doc["brief"]
        if not self.brief.endswith("."):
            self.brief += "."

        self.counters: dict = dict.fromkeys(_MAPPING.keys(), 0)

    def generate_documentation(self) -> str:
        header = (
            "S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S"
        )
        footer = (
            "S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S"
        )

        result = header + "\n"
        result += "  Struct:   " + self.name + "\n\n"
        result += "  Summary:  " + self.brief + "\n"

        last_directive = "~"
        data = self.doc["data"]
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

        result = result.rstrip("\n")
        result += "\n" + footer

        return result
