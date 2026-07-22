from pathlib import Path

from . import COPYRIGHT_NOTICE
from ._function import Function
from ._struct import Struct


class File:
    def __init__(self, path: Path):
        self.path = path

        self.functions: list = []
        self.structs: list = []

    def add_fn(self, fn: Function):
        self.functions.append(fn)

    def add_struct(self, struct: Struct):
        self.structs.append(struct)

    def generate_documentation(self, output_path: Path) -> None:
        output_path.mkdir(exist_ok=True, parents=True)
        if self.functions or self.structs:
            dir_name = output_path / (self.path.name + ".dir")
            dir_name.mkdir(parents=True, exist_ok=True)

            for i in self.functions + self.structs:
                doc = i.generate_documentation()
                file_name = dir_name / (i.name + ".txt")

                with file_name.open("w", encoding="utf-8") as f:
                    f.write(doc)

        if not (self.functions or self.structs):
            return

        # build file header
        header = "+==================================================================="
        footer = "===================================================================+"

        result = header + "\n"
        result += f"  File:      {self.path.name.upper()}\n"

        result += self._print_items(self.functions, "Functions")
        result += self._print_items(self.structs, "Structs")

        result = result.rstrip("\n")

        result += "\n\n"
        result += "  " + COPYRIGHT_NOTICE
        result += "\n" + footer

        self_output = output_path / (self.path.name + ".txt")
        with self_output.open("w", encoding="utf-8") as f:
            f.write(result)

    def _print_items(self, items: list, name: str) -> str:
        result = ""
        if not items:
            return ""

        name += ":"
        result += f"\n  {name:11}"
        first = items[0]
        result += first.name + " - " + first.brief + "\n"

        for idx in range(1, len(items)):
            item = items[idx]
            result += "             "
            result += item.name + " - " + item.brief + "\n"

        return result
