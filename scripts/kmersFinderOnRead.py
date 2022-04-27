import argparse


def kmers_file_to_kmers_set(filename_kmers):
    kmers = set()
    with open(filename_kmers, "r") as fichier:
        for ligne in fichier:
            kmers.add(ligne.split()[0])
    return kmers


def grep(filename, the_set, k, out):
    with open(out, "w") as output:
        with open(filename, "r") as fichier:
            for ligne in fichier:
                kmers = set()
                for start in range(len(ligne) - k + 1):
                    kmer = ligne[start : start + k]
                    if kmer in the_set:
                        kmers.add(kmer)
                if kmers:
                    output.write(ligne)
                    output.write("-> " + kmer + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Take a file of kmers and say where they appear in a fast(a/q) file"
    )
    parser.add_argument("filename_kmers", type=str, help="the filename of kmers")
    parser.add_argument(
        "filename_fast", type=str, help="the filename of the fast(a/q) file"
    )
    parser.add_argument("k", type=str, help="value of k")
    parser.add_argument("output", type=str, help="output filename")

    args = parser.parse_args()

    filename_kmers = args.filename_kmers
    filename_fast = args.filename_fast
    k = int(args.k)
    out = args.output

    kmers = kmers_file_to_kmers_set(filename_kmers)
    grep(filename_fast, kmers, k, out)


if __name__ == "__main__":
    main()
