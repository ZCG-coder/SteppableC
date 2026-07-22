from pathlib import Path
from typing import List, Tuple

import pyparsing as pp

from ._file import File
from ._function import Function
from ._struct import Struct
from ._parse_fn import parse_c_signature
from ._parse_struct import parse_c_struct
from ._parse_doc import parse_doc


def _process_fn(doc_comment: str, signature_parsed: dict) -> Function:
    name = signature_parsed["function_name"]
    return_type = signature_parsed["return_type"]
    args = signature_parsed["arguments"]
    doc_parsed = parse_doc(doc_comment)

    return Function(
        name=name,
        return_type=return_type,
        args=args,
        doc=doc_parsed,
    )


def _process_struct(doc_comment: str, signature_parsed: dict) -> Struct:
    name = signature_parsed["name"]
    fields = signature_parsed["fields"]
    doc_parsed = parse_doc(doc_comment)

    return Struct(
        name=name,
        fields=fields,
        doc=doc_parsed,
    )

def _process_file(file: Path) -> File:
    doc_comment_encountered = False
    doc_comment = ""

    can_read_signature = False
    signature = ""

    current_file_obj = File(file)

    with file.open("r", encoding="utf-8") as f:
        while line := f.readline():
            line = line.strip()

            if can_read_signature:
                # doc comment and signature read. done for this one.
                if parsed := parse_c_signature(signature):
                    can_read_signature = False

                    fn = _process_fn(doc_comment, parsed)
                    current_file_obj.add_fn(fn)

                    signature = ""
                    doc_comment = ""
                    continue

                if parsed := parse_c_struct(signature):
                    can_read_signature = False

                    struct = _process_struct(doc_comment, parsed)
                    current_file_obj.add_struct(struct)

                    signature = ""
                    doc_comment = ""
                    continue

                if line.startswith("/**"):
                    print("E003 - missing or invalid signature for doc comment")
                    exit(1)

                # keep looking
                signature += line + "\n"
                continue

            if doc_comment_encountered:
                if line.startswith("/**"):
                    print("E002 - incorrect doc comment format")
                    print("E002 - duplicate start")
                    exit(1)

                # Fix 5: Flexible end delimiter check
                if line.startswith("*/") or line.endswith("*/"):
                    doc_comment_encountered = False
                    doc_comment = doc_comment.strip("\n")

                    if doc_comment == "":
                        print("E002 - incorrect doc comment format")
                        print("E002 - empty comment")
                        exit(1)

                    can_read_signature = True
                    continue

                # in the middle of it
                if line.startswith("*"):
                    line = line[1:]
                    line = line.lstrip()

                doc_comment += line + "\n"
                continue

            # Fix 5: Flexible start delimiter check
            if line.startswith("/**"):
                doc_comment_encountered = True

        if can_read_signature:
            if parsed := parse_c_signature(signature):
                fn = _process_fn(doc_comment, parsed)
                current_file_obj.add_fn(fn)
            elif parsed := parse_c_struct(signature):
                struct = _process_struct(doc_comment, parsed)
                current_file_obj.add_struct(struct)
            else:
                print("E003 - missing or invalid signature for doc comment at EOF")
                exit(1)

    return current_file_obj

def process_file(file: Path) -> File:
    res = []
    try:
        res = _process_file(file)
    except FileNotFoundError:
        print("E002 - unable to open file")
        print(f"{file} is not found")
        exit(1)
    except UnicodeDecodeError:
        print("E003 - unable to decode file as utf-8")
        print(f"{file} contains invalid unicode")
        exit(1)

    return res
