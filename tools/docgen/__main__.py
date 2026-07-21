from ._parse_c import parse_c_signature
from ._read_dir import process_dir
from . import COPYRIGHT_NOTICE

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description="Generate documentation for C files",
        epilog=COPYRIGHT_NOTICE,
    )

    parser.add_argument(
        "-o",
        "--output-directory",
        help="directory to write documentation to",
        default="docs",
    )
    parser.add_argument(
        "-I",
        "--include",
        help="include directories to go through",
        action="append",
        default=[],
    )

    args = parser.parse_args()
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


if __name__ == "__main__":
    main()
