from pathlib import Path

from ._function import Function
from . import COPYRIGHT_NOTICE


class File:
    def __init__(self, path: Path):
        self.path = path

        self.functions: list = []

    def add_fn(self, fn: Function):
        self.functions.append(fn)

    def generate_documentation(self, output_path: Path) -> None:
        dir_name = output_path / (self.path.name + ".dir")
        dir_name.mkdir(parents=True, exist_ok=True)

        for i in self.functions:
            doc = i.generate_documentation()
            file_name = dir_name / (i.name + ".txt")

            with file_name.open("w", encoding="utf-8") as f:
                f.write(doc)

        # build file header
        header = "+==================================================================="
        footer = "===================================================================+"

        result = header + "\n"
        result += f"  File:      {self.path.name.upper()}\n\n"

        if self.functions:
            result += "  Functions: "
            first = self.functions[0]
            result += first.name + " - " + first.brief + "\n"

            for idx in range(1, len(self.functions)):
                fn = self.functions[idx]
                result += "             "
                result += fn.name + " - " + fn.brief + "\n"

        result = result.rstrip("\n")

        result += "\n\n"
        result += "  " + COPYRIGHT_NOTICE
        result += "\n" + footer

        self_output = output_path / (self.path.name + ".txt")
        with self_output.open("w", encoding="utf-8") as f:
            f.write(result)
