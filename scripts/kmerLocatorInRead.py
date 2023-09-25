"""
Take a file looking like

CGCGCGCGCGCGTGACGTTACCCAGGATAG
-> kmer from the read

and print the % of kmers in extremity
"""
import argparse


def is_in_start_or_end(read, kmer):
    """Return True if kmer is in the start or the end of the read, False otherwise"""
    location = read.find(kmer)
    start_of_the_last_kmer = len(read) - len(kmer)

    if location == -1:
        print("error:", kmer, "not in ", read)
        return False  # should I exit ?
    elif location == 0 or location == start_of_the_last_kmer:
        return True
    else:
        return False


def complexity(sequence):
    """Computes the complexity of a kmer"""
    k = 4
    kmers = set()
    nb_kmers = len(sequence) - k + 1
    kmers = {sequence[i : i + k] for i in range(nb_kmers)}
    return len(kmers) / nb_kmers


def is_in_start_or_end2(read, kmer):
    """
    Return True if kmer is in the start or the end of the read, False otherwise.
    Readability++, speed --
    """
    if read.startswith(kmer) or read.endswith(kmer):
        return True
    if kmer in read:
        return False
    else:
        print("error:", kmer, "not in ", read)
        return False  # should I exit ?


def get_number_of_line_and_extremity_kmer_along_complexity(filename):
    in_start_or_end = 0
    nb_line = 0
    complexities = {}
    with open(filename, "r") as fichier:
        read = ""
        for ligne in fichier:
            ligne = ligne.strip()
            if ligne:

                if ligne.startswith("-> "):
                    kmer = ligne[3:]
                    compl = int(complexity(kmer) * 100)
                    if compl in complexities:
                        complexities[compl] += 1
                    else:
                        complexities[compl] = 1
                    # print(kmer, compl)
                    if not read:
                        pass
                    if is_in_start_or_end(read, kmer):
                        in_start_or_end += 1
                else:
                    nb_line += 1
                    read = ligne
    return nb_line, in_start_or_end, complexities


def main():
    parser = argparse.ArgumentParser(
        description="Take a file like mentionned in docstring of this file."
    )
    parser.add_argument("filename", type=str, help="input filename")

    args = parser.parse_args()

    filename = args.filename

    (
        nb_line,
        in_start_or_end,
        complexities,
    ) = get_number_of_line_and_extremity_kmer_along_complexity(filename)
    print(nb_line, in_start_or_end)
    print()

    keys = sorted(complexities.keys())
    for key in keys:
        print(key, complexities[key])


if __name__ == "__main__":
    main()

