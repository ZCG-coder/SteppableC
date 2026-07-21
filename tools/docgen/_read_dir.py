from pathlib import Path

from ._read_file import process_file


def process_dir(directory: Path, output_dir: Path):
    for item in directory.iterdir():
        if item.is_dir():
            _process_dir(item, output_dir / (item.name + ".dir"))

        # restrict to *.c and *.h only
        suffix = item.suffix
        suffix = suffix.lower()
        if suffix == ".c" or suffix == ".h":
            res = process_file(item)
            res.generate_documentation(output_dir)

            print("processed", item.name)
