from math import log
import argparse

# TODO add nb_hash_function


def blocks(files, size=65536):
    while True:
        b = files.read(size)
        if not b:
            break
        yield b


def get_nb_lines_of(filename):
    with open(filename, "r") as f:
        return sum(bl.count("\n") for bl in blocks(f))


def main():
    parser = argparse.ArgumentParser(
        description="Computes the FPR of a Bloom filter with ONE hash function."
    )
    parser.add_argument(
        "epsilon",
        type=float,
        help="the FPR of the filter (0->1)",
    )
    parser.add_argument(
        "filename",
        type=str,
        help="the name of the file containing the Kmers",
    )
    parser.add_argument(
        "--script",
        type=bool,
        help="use if only the space using ine hash function is needed. Useful is called from a script.",
        default=False,
        required=False
    )
    args = parser.parse_args()

    epsilon = args.epsilon  # in 0->1
    filename = args.filename

    n = get_nb_lines_of(filename)
    mopti = int(-(n * log(epsilon)) / (log(2) ** 2))
    m1 = int(1 / (1 - (1 - epsilon) ** (1 / n)))
    if args.script:
        print(m1)
    else:
        print("optimal number of hash function, epsilon =", epsilon, "number of Kmers =", n)
        print("space in bits :", mopti)

        print()

        
        print("1 hash function, epsilon =", epsilon, "number of Kmers =", n)
        print("space in bits :", m1)


if __name__ == "__main__":
    main()
