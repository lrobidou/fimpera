from math import log
import argparse
import matplotlib.pyplot as plt



def main():
    parser = argparse.ArgumentParser(
        description="Computes rate of GC in files."
    )
    parser.add_argument(
        "filenames",
        type=str,
        nargs="+",
        help="the name of the file containing the kmers. each line should contains just one kmer (no abundance, for instance)",
    )
    args = parser.parse_args()

    filenames = args.filenames
    txGC_to_nb_times = {}
    for filename in filenames:
        print(filename)
        
        with open(filename, 'r') as fichier:
            nbLine = 0
            for ligne in fichier:
                nbLine += 1
                A = ligne.count("A")
                C = ligne.count("C")
                T = ligne.count("T")
                G = ligne.count("G")
                tx = (G+C) / len(ligne)
                if tx > 1:
                    print(tx)
                if not tx in txGC_to_nb_times:
                    txGC_to_nb_times[tx] = 0
                txGC_to_nb_times[tx] += 1
            for tx in txGC_to_nb_times:
                txGC_to_nb_times[tx] = txGC_to_nb_times[tx] / nbLine

        
        les_x = sorted(list(txGC_to_nb_times.keys()))
        les_y = []
        for key in les_x:
            les_y.append(txGC_to_nb_times[key])
        if "fp" in filename:
            plt.plot(les_x,les_y, "--", label = filename)
        else:
            plt.plot(les_x,les_y, "-", label = filename)
    plt.xlabel('(G+C) / len(kmer) ')
    plt.ylabel('nb kmer having that rate of GC')
    plt.legend(loc='best')
    plt.show()








if __name__ == "__main__":
    main()
