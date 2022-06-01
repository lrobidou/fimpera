#pragma once
#include <assert.h>
#include <robin_hood.h>

#include <algorithm>
#include <deque>
#include <fimpera-lib/canonical.hpp>
#include <iostream>
#include <string>
#include <vector>

// search the minimum in every sliding windows from vector ARR.
// **Erases the ARR vector.**
// see paper for explanation on how it works
inline std::vector<int> sliding_window_minimum(std::vector<int>& ARR, uint64_t w) {
    // basically, the minimum in the sliding window is the minimum of two "offseted" vectors (min_left and min_right)
    // but:
    // min_left is computed on the fly (we only store its required element)
    // min_right is stored in the vector passed as parameter
    // the response is computed in the ARR vector directly to avoid memory allocation.
    // OPTIMIZE throw errors instead of returning empty response
    if (ARR.size() < w) {
        return {};
    }
    if (w == 0) {
        return ARR;
    }
    if (w == 1) {
        return ARR;
    }

    uint64_t nbWin = ARR.size() / w;
    int nb_elem_last_window = ARR.size() % w;

    int min_left = ARR[0];
    for (uint64_t i = 0; i < w; i++) {
        min_left = std::min(min_left, ARR[i]);
    }

    for (uint64_t i = 0; i < nbWin - 1; i++) {
        int start_window = i * w;
        for (int indice = start_window + w - 2; indice >= start_window; indice--) {
            // we compute "min_right" here, directly in ARR vector
            ARR[indice] = std::min(ARR[indice + 1], ARR[indice]);
        }

        for (uint64_t j = 0; j < w; j++) {
            ARR[start_window + j] = std::min(ARR[start_window + j], min_left);
            min_left = (j == 0) ? ARR[start_window + w + j] : std::min(min_left, ARR[start_window + w + j]);
        }
    }

    // last window

    // compute min_right for last window
    int start_window = (nbWin - 1) * w;
    for (int indice = start_window + w - 2; indice >= start_window; indice--) {
        ARR[indice] = std::min(ARR[indice + 1], ARR[indice]);
    }

    // compute the min for the last window
    for (int j = 0; j < nb_elem_last_window; j++) {
        ARR[start_window + j] = std::min(ARR[start_window + j], min_left);
        min_left = (j == 0) ? ARR[start_window + w + j] : std::min(min_left, ARR[start_window + w + j]);
    }

    ARR[start_window + nb_elem_last_window] = std::min(ARR[start_window + nb_elem_last_window], min_left);
    for (std::size_t i = 0; i < w - 1; i++) {
        ARR.pop_back();
    }

    return ARR;
}

template <typename T>
inline int doQuery(const T& amqc, const std::string& kmer, bool canonical) {
    if (canonical) {  // TODO test
        return amqc.get(toCanonical(kmer));
    }
    return amqc.get(kmer);
}

inline bool isInvalid(const std::string& sequence) {
    // check if 'n' is in the read: return that the kmer is invalid
    if (sequence.find('n') != std::string::npos) {
        return true;
    }
    int k = 5;
    robin_hood::unordered_set<std::string> kmers;
    int nb_kmers = sequence.size() - k + 1;
    for (int start = 0; start < nb_kmers; start++) {
        kmers.insert(sequence.substr(start, k));
    }

    float complexity = ((float)kmers.size()) / nb_kmers;
    return complexity < 0.75;
}

// TODO even more tests
// OPTIMIZE do min in place (expected gain: *2 speed ratio ?)
/** // TODO redo description
 * @brief Query using "counting findere".
 * @param filterOrTruth the amq wrapped within a customAMQ
 * @param s the sequence to be queried
 * @param K the value for (big) k
 * @param z the current position being queried in s
 * @param canonical should kmers be queried in their canonical form ?
 * @param filtration should low complexity kmers be skipped ?
 * @param uppercase should kmers be turned into uppercase ?
 * @return The result of fimpera's query on s. Low complexity kmers are answered as -1.
 */
template <typename T>
inline std::vector<int> finderec(const T& amqc, const std::string& query, const unsigned int& K, const unsigned int& z, const bool& canonical, const bool& filtration = true, const bool& uppercase = true) {
    if (query.length() < K) {
        // throw std::range_error("`query.length()` (which is " + std::to_string(query.length()) + ") < `K` (which is " + std::to_string(K) + ")");
        std::cerr << "`query.length()` (which is " + std::to_string(query.length()) + ") < `K` (which is " + std::to_string(K) + ") : " << query << std::endl;
        return std::vector<int>();
    }

    std::string uppercase_query = query;
    if (uppercase) {
        std::transform(uppercase_query.begin(),
                       uppercase_query.end(),
                       uppercase_query.begin(),
                       [](unsigned char c) { return std::toupper(c); });
    }

    const unsigned int k = K - z;
    unsigned long long size = uppercase_query.size();
    std::vector<int> response(size - K + 1, 0);
    unsigned long long stretchLength = 0;  // number of consecutive positives kmers
    unsigned long long j = 0;              // index of the query vector
    bool extending_stretch = true;
    int amq_answer = 0;
    std::vector<int> previous_answers;
    previous_answers.reserve(response.size());
    while (j < size - k + 1) {
        assert(stretchLength == previous_answers.size());
        if ((amq_answer = doQuery(amqc, uppercase_query.substr(j, k), canonical))) {
            if (extending_stretch) {
                previous_answers.push_back(amq_answer);
                stretchLength++;
                j++;
            } else {
                extending_stretch = true;
                j = j - z;
            }
        } else {
            if (stretchLength > z) {
                unsigned long long t = j - stretchLength;
                for (const auto& minimum : sliding_window_minimum(previous_answers, z + 1)) {
                    response[t++] = minimum;
                }
            }
            previous_answers.clear();
            stretchLength = 0;
            extending_stretch = false;
            j = j + z + 1;
        }
    }
    // Last values:
    if (stretchLength > z) {
        unsigned long long t = size - k + 1 - stretchLength;
        for (const auto& minimum : sliding_window_minimum(previous_answers, z + 1)) {
            response[t++] = minimum;
        }
    }

    if (filtration) {
        // correction of invalid kmers:
        // OPTIMIZE: do not perform query instead of correcting it
        for (std::size_t i = 0; i < response.size(); i++) {
            if (isInvalid(uppercase_query.substr(i, K))) {
                response[i] = -1;
            }
        }
    }

    return response;
}
