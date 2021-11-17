# TODO: move this file ?


def min_sliding_window_naive(a, k):
    res = []
    min = a[0]
    i_min = 0

    # init the first window
    for j in range(1, k):
        if a[j] < min:
            min = a[j]
            i_min = j
    res.append(min)

    # update for all other windows
    for i in range(k, len(a)):
        if i_min == i - k:
            min = a[i - k + 1]  # set a new min
            i_min = i - k + 1
            for j in range(
                i - k + 2, i
            ):  # do not test the entering position i (tested later)
                if a[j] < min:
                    min = a[j]
                    i_min = j
        if a[i] <= min:
            min = a[i]
            i_min = i
        res.append(min)

    return res


def window_i(arr, k, i):
    start_window = i * k

    end_window = start_window + k
    # print(arr[start_window:end_window])
    window = []
    indice = end_window - 1
    minimum = arr[indice]
    window.append(minimum)
    while indice > start_window:
        indice -= 1
        if minimum > arr[indice]:
            minimum = arr[indice]
        window.append(minimum)
    return window[::-1]


def my_min(a, k):
    sliding_min = []
    y = a[0]
    for j in range(k):
        y = min(y, a[j])
    for i in range((len(a) // k) - 1):
        window = window_i(a, k, i)
        # print(window)
        for j in range(k):
            sliding_min.append(min(y, window[j]))
            if j == 0:
                y = a[i * k + k + j]
            else:
                y = min(y, a[i * k + k + j])  # TODO dernier indice

    # last window
    i = (len(a) // k) - 1

    we_go_up_to = len(a) % k + 1
    window = window_i(a, k, i)
    for j in range(we_go_up_to - 1):
        sliding_min.append(min(y, window[j]))
        if j == 0:
            y = a[i * k + k + j]
        else:
            # print(i * k + k + j)
            y = min(y, a[i * k + k + j])
    sliding_min.append(min(y, window[we_go_up_to - 1]))
    return sliding_min


l = [
    8,
    36,
    75,
    35,
    76,
    81,
    84,
    65,
    44,
    34,
    24,
    69,
    90,
    34,
    14,
    94,
    19,
    61,
    22,
    11,
    48,
    12,
    69,
    90,
    34,
    14,
    94,
    19,
    61,
    22,
    11,
    69,
    90,
    34,
    14,
    94,
    19,
    61,
    22,
    9,
    69,
    90,
    34,
    14,
    94,
    19,
    61,
    22,
    220,
    1,
]


def main():
    k = 8

    print("len(l) =", len(l))
    # for i in range((len(l) // 8)):
    #     start_window = i * k
    #     end_window = start_window + k
    #     print(l[start_window:end_window])
    # print("===============")
    l0 = min_sliding_window_naive(l, 8)
    l1 = my_min(l, 8)
    print()
    print("naive way:             ", l0)
    print("dynamic programming way", l1)
    print()
    print(l0 == l1)


if __name__ == "__main__":
    main()