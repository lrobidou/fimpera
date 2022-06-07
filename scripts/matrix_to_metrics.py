import argparse
import matplotlib.pyplot as plt

"""
Take a list of z and search for files named {z}.txt that contains the matrix representation of fimpera's response for z={z}.
Plot three graphs per value of z:
- FRP graph (pie)
- % of correct abundance graph (pie)
- mustache plot of error of abundance (mustache)

graphs are printed to files named: {z}_fpr_proportion.png, {z}_correct_abundance_proportion.png and {z}_mustache.png
"""


def get_matrix(filename):
    data = []
    firts_line = True
    with open(filename, "r") as fichier:
        for ligne in fichier:
            dataline = [int(x) for x in ligne.strip().split()]
            data.append(dataline)
            if firts_line:
                firts_line = False
                lendata = len(dataline)
            elif len(dataline) != lendata:
                print(dataline, "has not a len of", lendata)
    return data


def func(pct, allvals):
    return "{:.3f}%".format(pct)


def plot_fpr(z, data, outfilename):
    print("plot_fpr")
    truth_zero = data[0]
    tn, fps = truth_zero[0], truth_zero[1:]
    fpr = sum(fps) / (sum(fps) + tn)
    print("fpr=", fpr)
    plt.clf()
    plt.pie(
        [1 - fpr, fpr],
        startangle=90,
        autopct=lambda pct: func(pct, data),
    )
    if z > 0:
        plt.title(f"fimpera's FPR for z={z}")
    else:
        plt.title(f"CBF's FPR (z={z}")
    plt.legend(["TPs", "FPs"], loc="center right")
    plt.savefig(outfilename)


def plot_abundance_how_many_ok(z, data, outfilename):
    print("plot_abundance_how_many_ok")

    sommeverite, sommesurestime = 0, 0
    for i, ligne in enumerate(data[1:]):
        i += 1
        verite, surestime = ligne[i], ligne[i + 1 :]
        sommeverite += verite
        sommesurestime += sum(surestime)

    plt.clf()
    plt.pie(
        [sommeverite, sommesurestime],
        startangle=90,
        autopct=lambda pct: func(pct, data),
    )
    if z > 0:
        plt.title(f"fimpera's proportion of correctly reported abundance for z={z}")
    else:
        plt.title(f"CBF's proportion of correctly reported abundance (z={z}")
    plt.legend(["correct abundance", "wrong abundance"], loc="center right")
    plt.savefig(outfilename)


def plot_mustache(z, data, outfilename):
    print("plot mustache")
    plt.clf()
    points_to_plot = []
    valid_i = []
    for i, ligne in enumerate(data[1:], 1):

        # [1, 5, 2] -> [0, 1, 1, 1, 1, 1, 2, 2]
        overestimated = ligne[i + 1 :]
        liste_overestimation = []
        for j, overestimation in enumerate(overestimated):
            liste_overestimation += [i + 1 + j] * (overestimation)

        if liste_overestimation:
            valid_i.append(i)
            points_to_plot.append(liste_overestimation)

    plt.boxplot(points_to_plot, labels=[str(i) for i in valid_i])
    if z > 0:
        plt.title(f"fimpera's overestimation of abundance for z={z}")
    else:
        plt.title(f"CBF's overestimation of abundance (z={z}")
    # plt.legend(loc="best")
    plt.xlabel("correct abundance")
    plt.ylabel("abundance reported")
    plt.savefig(outfilename)


def main():
    parser = argparse.ArgumentParser(
        description="Take a list of files containing a matrix and plot 3 metrics to pngs."
    )
    parser.add_argument("zvalues", nargs="+", type=str, help="list of z values")

    args = parser.parse_args()

    zvalues = [int(x) for x in args.zvalues]
    for z in zvalues:
        print(f"z = {z}")
        filename = str(z) + ".txt"
        base_filename_out = str(z)

        data = get_matrix(filename)

        # fpr graph
        plot_fpr(z, data, base_filename_out + "_fpr_proportion.png")
        # how many answers (that are not FP) have the correct abundance ?
        plot_abundance_how_many_ok(
            z, data, base_filename_out + "_correct_abundance_proportion.png"
        )
        # among the answers that are neither FP nor have the correct abundance, what are the reported abundances wrt the real abundance ?
        plot_mustache(z, data, base_filename_out + "_mustache.png")

        # print \n if not the last z value
        if z != zvalues[-1]:
            print()


if __name__ == "__main__":
    main()
