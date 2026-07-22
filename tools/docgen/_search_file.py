from pathlib import Path


def search_file(name: str, directory: Path, ignore_case: bool = False) -> list:
    matches = []

    for item in directory.iterdir():
        if item.is_dir():
            matches += search_file(name, item, ignore_case)
            continue

        item_name = item.name
        current_name = name
        if ignore_case:
            item_name = item_name.lower()
            name = name.lower()

        if item_name == name:
            matches.append(item)

    return matches
