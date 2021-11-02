from pathlib import Path


def main():
    for path in Path(".").rglob("*.hpp"):
        with open(path, "r") as f:
            first_line = f.readline()
            if first_line != "#pragma once\n":
                print(path)


if __name__ == "__main__":
    main()