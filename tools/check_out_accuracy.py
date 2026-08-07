"""Checks accuracy of add, sub, mul, div outputs"""

import argparse
import decimal
import math

SUPPORTED_OPS = {
    "add": lambda x, y: x + y,
    "sub": lambda x, y: x - y,
    "mul": lambda x, y: x * y,
    "div": lambda x, y: x / y,
    "exp": lambda x: x.exp(),
    "sqrt": lambda x: x.sqrt(),
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
    nargs = fn.__code__.co_argcount

    prec = int(args.prec)
    decimal.getcontext().prec = prec * 4
    decimal.getcontext().rounding = decimal.ROUND_HALF_UP

    line_counter = 0
    inputs = []
    result: decimal.Decimal
    with open(filename, "r", encoding="utf-8") as f:
        while l := f.readline():
            l = l[:-1]
            if line_counter <= nargs - 1:
                n = decimal.Decimal(l)
                inputs.append(n)
                line_counter += 1
                continue

            result = decimal.Decimal(l)
            line_counter = 0

            # Perform operation
            py_result = fn(*inputs)
            try:
                py_result = round(py_result, prec)
            except decimal.InvalidOperation as e:
                pass  # not enough digits to round to

            diff = abs(result - py_result)
            equal = diff < 10 ** (-prec)
            if not equal:
                print("FAIL")
                for idx, j in enumerate(inputs):
                    print(f"[IN]{idx:4d} = {j:f}")

                print(f"[OUT]    = {result:f}")
                print(f"[ANS]    = {py_result:f}")
                inputs = []

                if py_result != 0:
                    print(f"[dif]    = {diff} ({diff / py_result * 100:.2f}%)")
                else:
                    print(f"[dif]    = --")
                break

            inputs = []
            print("PASS", end="\r")

    print()


if __name__ == "__main__":
    main()
