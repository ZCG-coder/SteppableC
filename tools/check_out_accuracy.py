"""Checks accuracy of add, sub, mul, div outputs"""

import argparse
from decimal import Decimal, getcontext

SUPPORTED_OPS = {
    "add": lambda x, y: x + y,
    "sub": lambda x, y: x - y,
    "mul": lambda x, y: x * y,
    "div": lambda x, y: x / y,
}


def main():
    parser = argparse.ArgumentParser(
        prog="check_out_accuracy",
        description="Check if Steppable outputs are accurate",
    )

    parser.add_argument("operation", choices=[*SUPPORTED_OPS.keys()])
    parser.add_argument("filename")
    parser.add_argument("--prec", default=152, required=False)

    args = parser.parse_args()
    opcode = args.operation
    fn = SUPPORTED_OPS[opcode]
    filename = args.filename

    getcontext().prec = args.prec * 2

    line_counter = 0
    lhs: Decimal
    rhs: Decimal
    result: Decimal
    with open(filename, "r", encoding="utf-8") as f:
        while l := f.readline():
            l = l[:-1]
            if line_counter == 0:
                lhs = Decimal(l)
            elif line_counter == 1:
                rhs = Decimal(l)
            elif line_counter == 2:
                result = Decimal(l)

            line_counter += 1
            if line_counter == 3:
                line_counter = 0

                # Perform operation
                py_result = fn(lhs, rhs)
                diff = abs(result - py_result)
                equal = diff < 10 ** (-args.prec + 1)
                if not equal:
                    print("FAIL")
                    print(f"LHS = {lhs:f}")
                    print(f"RHS = {rhs:f}")
                    print(f"res = {result:f}")
                    print(f"cor = {py_result:f}")
                    print(f"dif = {diff} ({diff / py_result * 100:.2f}%)")
                    break

                print("PASS", end="\r")

    print()


if __name__ == "__main__":
    main()
