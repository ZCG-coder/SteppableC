from pathlib import Path
import pyparsing as pp
from typing import List, Tuple

from ._parse_c import parse_c_signature
from ._parse_doc import parse_doc
from ._file import File
from ._function import Function


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
                # look for end of function signatures
                if ";" in line or "{" in line:
                    # doc comment and signature read. done for this one.
                    if parsed := parse_c_signature(signature):
                        can_read_signature = False

                        fn = _process_fn(doc_comment, parsed)
                        current_file_obj.add_fn(fn)

                        signature = ""
                        doc_comment = ""
                        continue

                # keep looking
                signature += line + "\n"
                continue

            if doc_comment_encountered:
                # end of doc comment
                if line == "*/":
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

            # start of doc comment
            if line == "/**":
                if doc_comment_encountered:
                    print("E002 - incorrect doc comment format")
                    print("E002 - duplicate start")
                    exit(1)

                doc_comment_encountered = True

    return current_file_obj


def process_file(file: Path) -> List[Tuple[dict, str]]:
    res = []
    try:
        res = _process_file(file)
    except FileNotFoundError:
        print("E002 - unable to open file")
        print(f"{FILE} is not found")
        exit(1)
    except UnicodeDecodeError:
        print("E003 - unable to decode file as utf-8")
        print(f"{FILE} contains invalid unicode")
        exit(1)

    return res
