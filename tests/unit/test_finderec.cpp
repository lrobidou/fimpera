#include <gtest/gtest.h>

#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/finderec.hpp>
#include <random>

inline void printv(const std::vector<int>& x, int i, int j) {
    for (int k = i; k < j; k++) {
        std::cout << x[k] << " ";
    }
    std::cout << std::endl;
}

template <typename T>
inline std::tuple<std::mt19937, std::uniform_int_distribution<T>> rd(T min, T max) {  // TODO move to utils
    std::random_device rd;                                                            // only used once to initialise (seed) engine
    std::mt19937 rng(rd());                                                           // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<T> uni(min, max);                                   // guaranteed unbiased
    return {rng, uni};
}

TEST(fimpera_test_suite_finderec, minElemInWindow) {
    std::vector<int> v = {
        0,    // 0
        4,    // 1
        5,    // 2
        6,    // 3
        1,    // 4
        2,    // 5
        3,    // 6
        7,    // 7
        8,    // 8
        5,    // 9
        112,  // 10
        6     // 11
    };

    EXPECT_EQ(minElemInWindow(v, 0, 3), 0);
    EXPECT_EQ(minElemInWindow(v, 5, 9), 2);
    EXPECT_EQ(minElemInWindow(v, 2, 9), 1);
    EXPECT_EQ(minElemInWindow(v, 0, 12), 0);
}

TEST(fimpera_test_suite_finderec, range_error) {
    int K = 31;
    int z = 0;
    std::string query = "ACT";
    std::vector<int> expected = {};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    std::string expected_error = "`query.length()` (which is " + std::to_string(query.length()) + ") < `K` (which is " + std::to_string(K) + ")";

    try {
        std::vector<int> res = finderec(cbf, query, K, z, false);
        FAIL() << "Expected std::range_error";
    } catch (std::range_error const& err) {
        EXPECT_EQ(err.what(), expected_error);
    } catch (...) {
        FAIL() << "Expected std::range_error";
    }
}

TEST(fimpera_test_suite_finderec, negative_stretch_size1) {
    int K = 31;
    int z = 0;
    std::string query = "ACTTTTTTTTTTTCGGTGCTGCTTAGGATAA";
    std::vector<int> expected = {0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_stretch_size9) {
    int K = 31;
    int z = 0;
    std::string query = "ACTTTTTTTTTTTCGGTGCTGCTTAGGATAAGAAAAAAA";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_1) {
    // no influence of a single positive kmer
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_2) {
    // be able to find a stretch
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789");
    cbf.set("23456789A");
    cbf.set("3456789AB");
    cbf.set("456789ABC");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_3) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789", 2);
    cbf.set("23456789A", 2);
    cbf.set("3456789AB", 2);
    cbf.set("456789ABC", 2);
    cbf.set("56789ABCD");
    cbf.set("6789ABCDE");
    cbf.set("789ABCDEF");
    cbf.set("9ABCDEFGH");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_4) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("9ABCDEFGH");
    cbf.set("ABCDEFGHI");
    cbf.set("BCDEFGHIJ");
    cbf.set("CDEFGHIJK");
    cbf.set("DEFGHIJKL");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}
TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_5) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("89ABCDEFG");
    cbf.set("9ABCDEFGH");
    cbf.set("ABCDEFGHI");
    cbf.set("BCDEFGHIJ");
    cbf.set("CDEFGHIJK");
    cbf.set("DEFGHIJKL");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}
TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_6) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("789ABCDEF");
    cbf.set("89ABCDEFG");
    cbf.set("9ABCDEFGH");
    cbf.set("ABCDEFGHI");
    cbf.set("BCDEFGHIJ");
    cbf.set("CDEFGHIJK");
    cbf.set("DEFGHIJKL");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, test_sliding_window_minimum) {
    const std::vector<int> in = {0, 1, 2, 3, 4, 5, 9, 9, 8, 2, 8, 5, 6, 2, 3, 4, 1};
    const std::vector<int> out = sliding_window_minimum(in, 5);
    const std::vector<int> expectation = {0, 1, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 1};
    EXPECT_EQ(out, expectation);
}

inline std::vector<int> sliding_window_minimum_dyn(const std::vector<int>& ARR, int w) {
    std::vector<int> sliding_min = std::vector<int>(ARR.size() - w + 1);

    int min_left = ARR[0];
    std::vector<int> min_right = std::vector<int>(w);

    int nbWin = ARR.size() / w;

    for (int i = 0; i < w; i++) {
        if (min_left > ARR[i]) {
            min_left = ARR[i];
        }
    }

    for (int i = 0; i < nbWin - 1; i++) {
        int start_window = i * w;
        int indice = start_window + w - 1;
        int minimum = ARR[indice];
        min_right[w - 1] = minimum;
        while (indice > start_window) {  // TODO use for
            indice--;
            if (minimum > ARR[indice]) {
                minimum = ARR[indice];
            }
            min_right[indice - start_window] = minimum;
        }
        for (int j = 0; j < min_right.size(); j++) {
            std::cout << min_right[j] << " ";
        }
        std::cout << std::endl;
        // on a le min_right
        for (int j = 0; j < w; j++) {
            if (min_right[j] < min_left) {
                sliding_min[i * w + j] = min_right[j];
            } else {
                sliding_min[i * w + j] = min_left;
            }
            if (j == 0) {
                min_left = ARR[i * w + w + j];
            } else {
                if (min_left > ARR[i * w + w + j]) {
                    min_left = ARR[i * w + w + j];
                }
            }
        }
    }

    // last window
    int i = nbWin - 1;
    int we_go_up_to = ARR.size() % w + 1;

    int start_window = i * w;
    int indice = start_window + w - 1;
    std::cout << "indice " << indice << std::endl;
    int minimum = ARR[indice];
    min_right[w - 1] = minimum;
    while (indice > start_window) {  // TODO for
        indice--;
        if (minimum > ARR[indice]) {
            minimum = ARR[indice];
        }
        min_right[indice - start_window] = minimum;
    }

    for (int j = 0; j < min_right.size(); j++) {
        std::cout << min_right[j] << " ";
    }
    std::cout << std::endl;

    for (int j = 0; j < we_go_up_to; j++) {
        if (min_right[j] < min_left) {
            sliding_min[i * w + j] = min_right[j];
        } else {
            sliding_min[i * w + j] = min_left;
        }
        if (j == 0) {
            min_left = ARR[i * w + w + j];
        } else {
            if (min_left > ARR[i * w + w + j]) {
                min_left = ARR[i * w + w + j];
            }
        }
    }
    return sliding_min;

    // // TODO what if size == K ?
    // for (int i = 0; i < w; i++) {
    //     min_left[i] = (i % w == 0) ? ARR[i] : std::min(min_left[i - 1], ARR[i]);
    // }

    // for (int i = 0; i < nbWin; i++) {
    //     startWin = i * w;
    //     endWin = (i + 1) * w;
    //     for (int j = 0; j < w; j++) {
    //         indice = startWin + j;
    //         min_left[indice] = (j % w == 0) ? ARR[indice] : std::min(min_left[indice - 1], ARR[indice]);

    //         int k = endWin - 1 - j;
    //         min_right[k] = (k % w == 0) ? ARR[k] : std::min(min_right[k + 1], ARR[k]);
    //     }
    //     for (int j = 0; j < w; j++) {
    //         indice = i * w + j;
    //         // sliding_min[indice] = std::min(min_right[indice], min_left[indice + w - 1]);
    //     }
    // }
    // TODO reste

    // int nbLeft = ARR.size() % w;
    // for (int i = 0; i < nbLeft; i++) {
    //     indice = nbWin * w + i;

    //     min_left[indice] = (indice % w == 0) ? ARR[indice] : std::min(min_left[indice - 1], ARR[indice]);

    //     int k = ARR.size() - indice - 1;
    //     min_right[k] = (i % w == 0) ? ARR[k] : std::min(min_right[k + 1], ARR[k]);
    // }

    // for (int i = 0, j = 0; i + w <= ARR.size(); i++) {
    //     sliding_min[j++] = std::min(min_right[i], min_left[i + w - 1]);
    // }

    // return sliding_min;
}

inline std::vector<int> sliding_window_minimum_naive(const std::vector<int>& ARR, int K) {
    std::vector<int> res;

    for (int j = 0; j < ARR.size() - K + 1; j++) {  // j : start of the window
        int min = ARR[j];
        for (int k = j; k < j + K; k++) {  // k : current position in the tab
            if (ARR[k] < min) {
                min = ARR[k];
            }
        }
        res.push_back(min);
    }
    return res;
}

inline std::vector<int> sliding_window_minimum_1(const std::vector<int>& ARR, int K) {
    std::deque<std::pair<int, int>> window;  // pair<int, int> represents the pair (ARR[i], i)
    std::vector<int> result;
    assert(ARR.size() >= K);
    result.reserve(ARR.size() - K);  // TODO: mesurer l'apport de ça, je suis curieux

    // initialisation step
    for (int i = 0; i < (K - 1); i++) {
        // clear the deque of the element higher than the current one
        while (!window.empty() && window.back().first >= ARR[i]) {
            window.pop_back();
        }
        // add the current element
        window.push_back(std::make_pair(ARR[i], i));
        // get rid of the "old" elements
        while (window.front().second <= i - K) {
            window.pop_front();
        }
    }

    // same, but we add the values to the vector
    for (int i = (K - 1); i < ARR.size(); i++) {
        while (!window.empty() && window.back().first >= ARR[i]) {
            window.pop_back();
        }
        window.push_back(std::make_pair(ARR[i], i));
        while (window.front().second <= i - K) {
            window.pop_front();
        }
        result.push_back(window.front().first);
    }
    return result;
}

inline std::vector<int> sliding_window_minimum_2(const std::vector<int>& ARR, int K) {
    std::vector<int> window_val(ARR.size());
    std::vector<int> window_i(ARR.size());
    int ind_min = 0;
    int ind_max = 0;
    std::vector<int> result;
    assert(ARR.size() >= K);
    result.reserve(ARR.size() - K);  // TODO: mesurer l'apport de ça, je suis curieux

    for (int i = 0; i < ARR.size(); i++) {
        while ((ind_max > ind_min) && window_val[ind_max - 1] >= ARR[i]) {
            ind_max--;
        }

        window_val[ind_max] = ARR[i];
        window_i[ind_max] = i;
        ind_max++;

        while (window_i[ind_min] <= i - K) {
            ind_min++;
        }

        if (i >= K - 1) {
            result.push_back(window_val[ind_min]);
        }
    }
    return result;
}
TEST(fimpera_test_suite_finderec, test_sliding_window_minimum_naive) {
    int size = 30;
    int window_size = 8;

    std::vector<int> in = {8, 36, 75, 35, 76, 81, 84, 65, 44, 34, 24, 69, 90, 34, 14, 94, 19, 61, 22, 11, 48, 12, 69, 90, 34, 14, 94, 19, 61, 22, 11, 69, 90, 34, 14, 94, 19, 61, 22, 9, 69, 90, 34, 14, 94, 19, 61, 22, 220};
    in.reserve(size);
    auto [rng, uni] = rd(0, 1000);
    // for (int i = 0; i < size; i++) {
    //     in.push_back(uni(rng));
    //     // std::cout << in[i] << " ";
    // }
    // std::cout << std::endl;

    using namespace std::chrono;

    // After function call
    auto start = high_resolution_clock::now();
    const std::vector<int> out0 = sliding_window_minimum_naive(in, window_size);
    auto naive1 = high_resolution_clock::now();
    const std::vector<int> out1 = sliding_window_minimum(in, window_size);
    auto naive2 = high_resolution_clock::now();
    const std::vector<int> out2 = sliding_window_minimum_1(in, window_size);
    auto end_1 = high_resolution_clock::now();
    const std::vector<int> out3 = sliding_window_minimum_2(in, window_size);
    auto end_2 = high_resolution_clock::now();
    const std::vector<int> out4 = sliding_window_minimum_dyn(in, window_size);
    auto end_3 = high_resolution_clock::now();

    auto duration_naive_1 = duration_cast<microseconds>(naive1 - start).count();
    auto duration_naive_2 = duration_cast<microseconds>(naive2 - naive1).count();
    auto duration_1 = duration_cast<microseconds>(end_1 - naive2).count();
    auto duration_2 = duration_cast<microseconds>(end_2 - end_1).count();
    auto duration_3 = duration_cast<microseconds>(end_3 - end_2).count();
    std::cout << "naive:  " << duration_naive_1 << std::endl;
    std::cout << "pierre: " << duration_naive_2 << std::endl;
    std::cout << "deque:  " << duration_1 << std::endl;
    std::cout << "tab:    " << duration_2 << std::endl;
    std::cout << "dyn:    " << duration_3 << std::endl;

    EXPECT_EQ(out0, out1);
    EXPECT_EQ(out0, out2);
    EXPECT_EQ(out0, out3);
    // for (int i = 0; i < out0.size(); ++i) {
    //     EXPECT_EQ(out0[i], out4[i]) << "Vectors out0 and out4 differ at index " << i << " " << out0[i] << " " << out4[i] << std::endl;
    // }

    EXPECT_EQ(out0, out4);
}