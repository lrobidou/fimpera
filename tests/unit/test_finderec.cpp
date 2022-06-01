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

template <typename T>
inline T minElemInWindow(std::vector<T> vect, int iMin, int iMax) {
    T minElem = vect[iMin];
    for (int i = iMin; i < iMax; i++) {
        T currentElem = vect[i];
        if (currentElem < minElem) {
            minElem = currentElem;
        }
    }
    return minElem;
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

// TEST(fimpera_test_suite_finderec, range_error) {
//     int K = 31;
//     int z = 0;
//     std::string query = "ACT";
//     std::vector<int> expected = {};
//     countingBF::CBF cbf = countingBF::CBF(1000, 5);
//     std::string expected_error = "`query.length()` (which is " + std::to_string(query.length()) + ") < `K` (which is " + std::to_string(K) + ")";
//
//     try {
//         std::vector<int> res = finderec(cbf, query, K, z, false);
//         FAIL() << "Expected std::range_error";
//     } catch (std::range_error const& err) {
//         EXPECT_EQ(err.what(), expected_error);
//     } catch (...) {
//         FAIL() << "Expected std::range_error";
//     }
// }

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
    std::vector<int> in = {0, 1, 2, 3, 4, 5, 9, 9, 8, 2, 8, 5, 6, 2, 3, 4, 1};
    const std::vector<int> out = sliding_window_minimum(in, 5);
    const std::vector<int> expectation = {0, 1, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 1};
    EXPECT_EQ(out, expectation);
}

inline std::vector<int> sliding_window_minimum_pierre(const std::vector<int>& ARR, int K) {
    std::vector<int> res;
    res.reserve(ARR.size() - K + 1);
    int min = ARR[0];
    std::size_t i_min = 0;

    for (int j = 1; j < K; j++) {
        if (ARR[j] < min) {
            min = ARR[j];
            i_min = j;
        }
    }
    res.push_back(min);

    for (std::size_t i = K; i < ARR.size(); i++) {
        if (i_min == i - K) {
            i_min = i - K + 1;
            min = ARR[i_min];
            for (std::size_t j = i - K + 2; j < i; j++) {
                if (ARR[j] < min) {
                    i_min = j;
                    min = ARR[i_min];
                }
            }
        }
        if (ARR[i] < min) {  // TODO pourquoi ça ne marche pas avec < strict ?
            i_min = i;
            min = ARR[i_min];
        }
        res.push_back(min);
    }
    return res;
}

inline std::vector<int> sliding_window_minimum_naive(const std::vector<int>& ARR, int K) {
    std::vector<int> res;

    for (std::size_t j = 0; j < ARR.size() - K + 1; j++) {  // j : start of the window
        int min = ARR[j];
        for (std::size_t k = j; k < j + K; k++) {  // k : current position in the tab
            if (ARR[k] < min) {
                min = ARR[k];
            }
        }
        res.push_back(min);
    }
    return res;
}

// inline std::vector<int> sliding_window_minimum_1(const std::vector<int>& ARR, int K) {
//     std::deque<std::pair<int, int>> window;  // pair<int, int> represents the pair (ARR[i], i)
//     std::vector<int> result;
//     assert(ARR.size() >= K);
//     result.reserve(ARR.size() - K);  // TODO: mesurer l'apport de ça, je suis curieux

//     // initialisation step
//     for (int i = 0; i < (K - 1); i++) {
//         // clear the deque of the element higher than the current one
//         while (!window.empty() && window.back().first >= ARR[i]) {
//             window.pop_back();
//         }
//         // add the current element
//         window.push_back(std::make_pair(ARR[i], i));
//         // get rid of the "old" elements
//         while (window.front().second <= i - K) {
//             window.pop_front();
//         }
//     }

//     // same, but we add the values to the vector
//     // TODO this segfaults
//     for (std::size_t i = (K - 1); i < ARR.size(); i++) {
//         while (!window.empty() && window.back().first >= ARR[i]) {
//             window.pop_back();
//         }
//         window.push_back(std::make_pair(ARR[i], i));
//         while (window.front().second <= i - K) {
//             std::cout << "start loop " << window.empty() << std::endl;
//             window.pop_front();
//             std::cout << "window " << window.front().first << " " << window.front().second << std::endl;
//             std::cout << "window " << window.empty() << std::endl;
//         }
//         std::cout << "out" << std::endl;
//         // result.push_back(window.front().first);
//     }
//     return result;
// }

inline std::vector<int> sliding_window_minimum_2(const std::vector<int>& ARR, int K) {
    std::vector<int> window_val(ARR.size());
    std::vector<std::size_t> window_i(ARR.size());
    std::size_t ind_min = 0;
    std::size_t ind_max = 0;
    std::vector<int> result;
    assert(ARR.size() >= K);
    result.reserve(ARR.size() - K);  // TODO: how much does that actually impact performance ?

    for (std::size_t i = 0; i < ARR.size(); i++) {
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

TEST(fimpera_test_suite_finderec, test_comparisons_sliding_window_minimum) {
    /*
    goal of this test: test multiple approaches of an algorithm (sliding_window_minimuum).
    Three implementations:
    - naive one: compute every window, then for each window, compute the min
    - pierre's one
    - dynamic programming-inspired solution (O(n), no heap allocation)
    */

    // test parameters
    int size = 300000;
    int window_size = 7;

    // create a random vector
    std::vector<int> in;
    in.reserve(size);
    auto [rng, uni] = rd(0, 1000);
    for (int i = 0; i < size; i++) {
        in.push_back(uni(rng));
    }

    using namespace std::chrono;

    // call each function
    auto start = high_resolution_clock::now();
    const std::vector<int> out0 = sliding_window_minimum_naive(in, window_size);
    auto naive1 = high_resolution_clock::now();
    const std::vector<int> out1 = sliding_window_minimum_pierre(in, window_size);
    auto end_2 = high_resolution_clock::now();
    const std::vector<int> out4 = sliding_window_minimum(in, window_size);
    auto end_3 = high_resolution_clock::now();

    // compute time taken
    auto duration_naive_1 = duration_cast<microseconds>(naive1 - start).count();
    auto duration_p = duration_cast<microseconds>(end_2 - naive1).count();
    auto duration_3 = duration_cast<microseconds>(end_3 - end_2).count();

    // silence unused variable warning
    (void)duration_naive_1;
    (void)duration_p;
    (void)duration_3;

    // uncomment to see time taken by each approach
    // std::cout << "naive:  " << duration_naive_1 << std::endl;
    // std::cout << "pierre: " << duration_p << std::endl;
    // std::cout << "dynamic programming:    " << duration_3 << std::endl;

    EXPECT_EQ(out0, out1);
    EXPECT_EQ(out0, out4);
}
