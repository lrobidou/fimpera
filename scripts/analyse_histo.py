def main():
    histo = {}
    histo_filename = "histo.txt"
    with open(histo_filename, "r") as fichier:
        for ligne in fichier:
            x, y = ligne.split()
            histo[int(x)] = int(y)
    number_of_kmer_correctly_answered = histo[0]
    number_of_kmer_incorrectly_answered = sum([histo[x] for x in histo if x != 0])
    print(number_of_kmer_correctly_answered)
    print(number_of_kmer_incorrectly_answered)
    print(
        number_of_kmer_correctly_answered
        / (number_of_kmer_correctly_answered + number_of_kmer_incorrectly_answered)
    )
    print([histo[x] for x in histo if x != 0])


if __name__ == "__main__":
    main()