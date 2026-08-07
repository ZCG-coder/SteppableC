import shutil
from pathlib import Path
from typing import List


def gh_process_dir(directory: Path, output: Path) -> List[Path]:
    output_items = []
    for item in directory.iterdir():
        if item.is_dir():
            output_items += gh_process_dir(item, output)
            continue

        rel_path = item.relative_to(output)
        rel_path = rel_path.with_suffix(".md")
        filename = "@".join(rel_path.parts)
        name = filename[:-3]

        contents = item.read_text(encoding="utf-8")
        contents = f"# `{item.stem}`\n\n```plaintext\n{contents}\n```"
        item.write_text(contents, encoding="utf-8")
        shutil.copy(
            str(item.resolve()),
            output / filename,
        )
        output_items.append((item.stem, name))


    return output_items
