import argparse
import matplotlib.pyplot as plt

plt.rcParams.update({"font.size": 14})
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


def get_fpr(matrix):
    truth_zero = matrix[0]
    tn, fps = truth_zero[0], truth_zero[1:]
    fpr = sum(fps) / (sum(fps) + tn)
    return fpr


def plot_all_fpr(data0, z, dataz, construction_dataz, outfilename):
    print("plot_all_fpr")
    plt.clf()

    barWidth = 0.5

    fpr0 = get_fpr(data0) * 100
    fprz = get_fpr(dataz) * 100
    cfpr = get_fpr(construction_dataz) * 100
    print("===================> proportion cFPR", (cfpr / fprz) * 100)
    # print(z, fpr0, fprz, cfpr)

    plt.bar(0, fpr0, width=barWidth, linewidth=2)
    plt.bar(1, fprz, width=barWidth, linewidth=2)

    plt.title(f"Proportion of false positives for z={z}")
    plt.xticks(list(range(2)), ["cBF", "fimpera"])
    plt.ylabel("false positive rate (%)")
    plt.savefig(outfilename)


def plot_all_incorrect_abundance_calls(
    data0, z, dataz, construction_dataz, outfilename
):
    print("plot_all_incorrect_abundance_calls")
    plt.clf()
    sommeverite0, sommesurestime0 = 0, 0
    for i, ligne in enumerate(data0[1:]):
        i += 1
        verite, surestime = ligne[i], ligne[i + 1 :]
        sommeverite0 += verite
        sommesurestime0 += sum(surestime)

    sommeveritez, sommesurestimez = 0, 0
    for i, ligne in enumerate(dataz[1:]):
        i += 1
        verite, surestime = ligne[i], ligne[i + 1 :]
        sommeveritez += verite
        sommesurestimez += sum(surestime)

    csommeverite, csommesurestime = 0, 0
    for i, ligne in enumerate(construction_dataz[1:]):
        i += 1
        verite, surestime = ligne[i], ligne[i + 1 :]
        csommeverite += verite
        csommesurestime += sum(surestime)
    barWidth = 0.5

    result0 = sommesurestime0 / (sommesurestime0 + sommeverite0) * 100
    resultz = sommesurestimez / (sommesurestimez + sommeveritez) * 100
    resultc = csommesurestime / (csommesurestime + csommeverite) * 100
    print(
        "===================> propotion construction overestimation (TPs only)",
        (resultc / resultz) * 100,
    )
    # print("%surestimation")
    # print(result0, resultz, resultc)
    # print(sommesurestime0, sommesurestimez)

    plt.bar(0, result0, width=barWidth, linewidth=2)
    plt.bar(1, resultz, width=barWidth, linewidth=2)

    plt.xticks(list(range(2)), ["cBF", "fimpera"])
    plt.ylabel("incorrect abundance call rate (%)")
    plt.title(f"Proportion of incorrect abundance calls for z={z}")
    plt.savefig(outfilename)


def plot_mustache(z, data, outfilename):
    print("plot mustache")
    plt.clf()
    points_to_plot = []
    valid_i = []
    avg_dist = 0
    sum_over = 0
    for i, ligne in enumerate(data[1:], 1):

        # [1, 5, 2] -> [0, 1, 1, 1, 1, 1, 2, 2]
        overestimated = ligne[i + 1 :]
        sum_over += sum(overestimated)

        liste_overestimation = []
        for j, overestimation in enumerate(overestimated):
            liste_overestimation += [i + 1 + j] * (overestimation)
            avg_dist += (j + 1) * overestimation

        if liste_overestimation:
            valid_i.append(i)
            points_to_plot.append(liste_overestimation)

    if valid_i:
        # print("avg dist", avg_dist / sum_over, avg_dist, sum_over)
        # print("------------")
        plt.boxplot(points_to_plot, labels=[str(i) for i in valid_i])
        if z > 0:
            plt.title(f"fimpera's overestimation of abundance for z={z}")
        else:
            plt.title(f"CBF's overestimation of abundance (z={z})")
        plt.xlabel("correct abundance")
        plt.ylabel("abundance reported")
        plt.savefig(outfilename)
    else:
        print("no mustache")


def main():
    parser = argparse.ArgumentParser(
        description="Take a list of z for which there is files containing a matrix and plot 3*3 metrics to pngs."
    )
    parser.add_argument("zvalues", nargs="+", type=str, help="list of z values")

    args = parser.parse_args()

    zvalues = [int(x) for x in args.zvalues]
    data0 = get_matrix("0.txt")
    for z in zvalues:
        print(f"z = {z}")
        filename = str(z) + ".txt"
        base_filename_out = str(z)

        construction_filename = str(z) + "_construction.txt"
        base_construction_filename_out = str(z) + "_construction"

        data = get_matrix(filename)
        construction_data = get_matrix(construction_filename)

        # among the answers that are neither FP nor have the correct abundance, what are the reported abundances wrt the real abundance ?
        plot_mustache(z, data, base_filename_out + "_mustache.png")

        plot_all_fpr(data0, z, data, construction_data, f"{z}_all_fp.png")
        plot_all_incorrect_abundance_calls(
            data0, z, data, construction_data, f"{z}_all_incorrect_abundance.png"
        )

        # plot mustache for construction error
        plot_mustache(
            z,
            construction_data,
            base_construction_filename_out + "_mustache.png",
        )

        # print \n if not the last z value
        if z != zvalues[-1]:
            print()


if __name__ == "__main__":
    main()
