from ._parse_c import parse_c_signature
from ._read_file import process_file

import argparse
from pathlib import Path

FILE = Path("stp_number/_add.c")

if __name__ == "__main__":
    res = process_file(FILE)
    print(res.generate_documentation())
