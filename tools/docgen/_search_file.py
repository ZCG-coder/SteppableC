from pathlib import Path


def search_file(name: str, directory: Path) -> list:
    matches = []

    for item in directory.iterdir():
        if item.is_dir():
            matches += search_file(name, item)
            continue

        if item.name == name:
            matches.append(item)

    return matches
