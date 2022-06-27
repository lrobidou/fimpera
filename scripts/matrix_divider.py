import argparse

"""
Take a file with the following structure:

[ignored]
starting analyzing z = 1 after 1451.75 s.
[ignored]
matrix_index_z_vs_truth
{matrix_that_will_be_isolated_in_its_own_file}
[ignored]
starting analyzing z = 3 after 4906.88 s.
[ignored]
matrix_index_z_vs_truth
{matrix_that_will_be_isolated_in_its_own_file}
[ignored]

Basically, lines "matrix_index_z_vs_truth" are detected and the matrix below it is printed in a separate output file . The output filename depends on the value of z in the line "starting analyzing z = {z}" before the matrix. Some lines can separate the declaration of z and the matrix, but no line can be put between "matrix_index_z_vs_truth" and the matrix content.
Lines that can be ignored are marked as [ignored]. Note that it can be multiple lines per [ignored] tag.
"""


def get_new_z_if_any(line):
    """Tries to return the value of z contained in the line. Returns false if no value of z is found."""
    # match line.strip().split():
    if line.startswith("starting analyzing"):
        line = line.split()

        new_z = line[4]
        print(new_z)
        return new_z
        # case "starting", "analyzing", "z", "=", new_z, "after", _, "s.":
        # return new_z
    return False


def this_line_is_the_start_of_a_z_matrix(line):
    if line.strip() == "matrix_index_z_vs_truth":
        return True
    return False


def this_line_is_the_start_of_a_construction_matrix(line):
    if line.strip() == "matrix_ctruth_vs_truth":
        return True
    return False


def is_vector(line):
    """Return True if line contains only number separated by space, False otherwise. Must contains at least one number
    Ignore comments after the first #."""
    splitted_line = line.strip().split()
    ignore = False  # No "#"" in this line yet
    if not len(splitted_line):
        return False
    for x in splitted_line:
        if x == "#":
            ignore = True
        if not ignore:
            if not x.isdigit():
                return False
    return True


def split_z_matrixes(filename):
    z = 0
    copy_in_file = False
    with open(filename, "r") as fichier:
        while ligne := fichier.readline():
            if copy_in_file:
                with open(f"{z}.txt", "w") as outfile:
                    if is_vector(ligne):
                        outfile.write(ligne)
                    while (ligne := fichier.readline()) and (
                        copy_in_file := is_vector(ligne)
                    ):
                        outfile.write(ligne)
            new_z = get_new_z_if_any(ligne)
            if new_z != False:
                z = new_z
            if this_line_is_the_start_of_a_z_matrix(ligne):
                copy_in_file = True


def split_construction_matrixes(filename):
    z = 0
    copy_in_file = False
    with open(filename, "r") as fichier:
        while ligne := fichier.readline():
            if copy_in_file:
                with open(f"{z}_construction.txt", "w") as outfile:
                    if is_vector(ligne):
                        outfile.write(ligne)
                    while (ligne := fichier.readline()) and (
                        copy_in_file := is_vector(ligne)
                    ):
                        outfile.write(ligne)
            new_z = get_new_z_if_any(ligne)
            if new_z != False:
                z = new_z
            if this_line_is_the_start_of_a_construction_matrix(ligne):
                copy_in_file = True


def main():
    parser = argparse.ArgumentParser(
        description="split matrixes of fimpera into different files, one per z value."
    )
    parser.add_argument("filename", type=str, help="the filename of matrixes")
    args = parser.parse_args()

    filename = args.filename
    split_z_matrixes(filename)
    split_construction_matrixes(filename)


if __name__ == "__main__":
    main()
