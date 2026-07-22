import re

from prompt_toolkit.formatted_text import HTML


def highlight_docblock(text: str, keyword: str) -> HTML:
    # escape HTML special chars
    html = text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

    more_keywords = re.findall(
        r"^(\s+)(Args:\s+)?([\w* ]+?)(\s+?)(\w+)$", html, re.MULTILINE
    )
    more_keywords = [x[4] for x in more_keywords]
    more_keywords.append(keyword)
    for kw in more_keywords:
        html = re.sub(
            rf"\b({kw})\b",
            r"<u>\1</u>",
            html,
            flags=re.MULTILINE | re.IGNORECASE,
        )

    # highlight arguments
    html = re.sub(
        r"^(\s+)(Args:\s+)?([\w* ]+?)(\s+?)<u>(\w+)</u>$",
        r"\1\2\3\4<ansiyellow>\5</ansiyellow>",
        html,
        flags=re.MULTILINE,
    )

    # dim borders
    html = re.sub(
        r"^(.+?)\n",
        r"<darkgray>\1</darkgray>\n",
        html,
    )
    html = re.sub(
        r"\n(.+?)$",
        r"\n<darkgray>\1</darkgray>",
        html,
    )

    # bold numerals and formatting asterisks
    html = re.sub(
        r"^(\s*)([A-Z][a-zA-Z0-9_]*:)(\s*)(\*)",
        r"\1\2\3<b><ansigray>\4</ansigray></b>",
        html,
        flags=re.MULTILINE,
    )
    html = re.sub(
        r"^(\s*)([A-Z][a-zA-Z0-9_]*:)(\s*)(\d+\.)",
        r"\1\2\3<b><ansigray>\4</ansigray></b>",
        html,
        flags=re.MULTILINE,
    )

    # highlight section headers
    html = re.sub(
        r"^(\s*)([A-Z][a-zA-Z0-9_]*:)",
        r"\1<b>\2</b>",
        html,
        flags=re.MULTILINE,
    )

    # highlight C/C++ types and qualifiers
    html = re.sub(
        r"\b(int|short|long|float|double|u?int\d+_t|char|null|NULL)\b",
        r"<ansiblue><b>\1</b></ansiblue>",
        html,
        flags=re.MULTILINE,
    )
    html = re.sub(
        r"\b(const|constexpr|consteval|static)\b",
        r"<ansibrightmagenta><b>\1</b></ansibrightmagenta>",
        html,
        flags=re.MULTILINE,
    )

    return HTML(html)
