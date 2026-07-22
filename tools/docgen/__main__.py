import argparse
from pathlib import Path

from prompt_toolkit.shortcuts import choice
from pypager.pager import Pager
from pypager.source import FormattedTextSource

from . import COPYRIGHT_NOTICE
from ._format_docpage import highlight_docblock
from ._read_dir import process_dir
from ._read_file import process_file
from ._search_file import search_file


def generate_parser_cb(args: argparse.Namespace):
    output = args.output_directory
    output = Path(output)

    if not args.include:
        print("E011 - no directories specified")
        print("E011 - please specify some directories")

        exit(1)

    for directory in args.include:
        directory = Path(directory)
        current_output = output / (directory.name + ".dir")
        process_dir(directory, current_output)


def readman_cb(args: argparse.Namespace):
    help_item = args.item
    directory = Path(args.search_directory)

    if not directory.is_dir():
        print("E012 - unable to open dir")
        print(f"E012 - {directory} is not a directory")
        exit(1)

    matches = search_file(f"{help_item}.txt", directory, args.ignore_case)

    selected_path: Path
    if len(matches) > 1:
        opts = [(idx, f"{match.stem} ({match.parent.stem})") for idx, match in enumerate(matches)]

        try:
            result = choice(
                message=f"{help_item}: multiple entries found. please select one",
                options=opts,
                default=0,
            )
        except KeyboardInterrupt:
            print("Exit")
            exit(1)

        selected_path = matches[result]
    elif len(matches) == 1:
        selected_path = matches[0]
    else:
        print("E021 - unable to find documentation")
        print(f"E021 - no entries found for {help_item}")
        exit(1)

    contents: str
    with selected_path.open("r", encoding="utf-8") as f:
        try:
            contents = f.read()
        except UnicodeDecodeError:
            print("E010 - unable to open documentation file")
            print("E010 - UnicodeDecodeError")
            exit(1)

    p = Pager()
    p.add_source(FormattedTextSource(highlight_docblock(contents, help_item)))
    p.run()


def main():
    parser = argparse.ArgumentParser(
        description="Generate documentation for C files",
        epilog=COPYRIGHT_NOTICE,
    )
    subparsers = parser.add_subparsers()

    generate_parser = subparsers.add_parser("generate", help="generate documentation")
    generate_parser.add_argument(
        "-o",
        "--output-directory",
        help="directory to write documentation to",
        default="docs",
    )
    generate_parser.add_argument(
        "-I",
        "--include",
        help="include directories to go through",
        action="append",
        default=[],
    )
    generate_parser.set_defaults(func=generate_parser_cb)

    readman_parser = subparsers.add_parser("man", help="read documentation generated")
    readman_parser.add_argument(
        "item",
        help="item to search for entries about",
    )
    readman_parser.add_argument(
        "-d",
        "--search-directory",
        help="directory to find documentation from",
        default="docs",
    )
    readman_parser.add_argument(
        "-n",
        "--ignore-case",
        help="ignore case while searching",
        action="store_true"
    )
    readman_parser.set_defaults(func=readman_cb)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
