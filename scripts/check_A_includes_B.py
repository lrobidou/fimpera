import argparse

# TODO: le fichier est mal nommé


def main():
    parser = argparse.ArgumentParser(
        description="Checks that smers from kmers from A are contained in B."
    )
    parser.add_argument(
        "filename_A",
        type=str,
        help="the filename A (k-mers)",
    )
    parser.add_argument(
        "filename_B",
        type=str,
        help="the filename B (s-mers)",
    )

    args = parser.parse_args()

    filename_a = args.filename_A
    filename_b = args.filename_B

    # set of all smers
    smers = {}
    with open(filename_b, "r") as fichier:
        first = True
        for ligne in fichier:
            ligne = ligne.strip().split()
            if first:
                s = len(ligne[0])
                first = False
            smers[ligne[0]] = int(ligne[1])

    # print(s)
    # print(smers)

    with open(filename_a, "r") as fichier:
        first = True
        for ligne in fichier:
            ligne = ligne.strip().split()
            if first:
                k = len(ligne[0])
                first = False
            for i in range(k - s + 1):
                smer = ligne[0][i : i + s]
                if int(ligne[1]) < smers[smer]:
                    print(smer, ligne[1], smers[smer])


if __name__ == "__main__":
    main()

