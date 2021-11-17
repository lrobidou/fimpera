#pragma once
#include <assert.h>  //TODO remove

#include <deque>
#include <iostream>
#include <string>
#include <vector>

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

// inspired by http://www.zrzahid.com/sliding-window-minmax/
inline std::vector<int> sliding_window_minimum(const std::vector<int>& ARR, int w) {
    std::vector<int> sliding_min = std::vector<int>(ARR.size() - w + 1);

    if (ARR.size() == 0) {
        return {};
    }
    if (w == 0) {
        return ARR;  // TODO erreur plutot
    }
    if (w == 1) {
        return ARR;
    }

    int min_left = ARR[0];
    std::vector<int> min_right = std::vector<int>(w);

    int nbWin = ARR.size() / w;

    for (int i = 0; i < w; i++) {
        min_left = std::min(min_left, ARR[i]);
    }

    for (int i = 0; i < nbWin - 1; i++) {
        int start_window = i * w;
        min_right[w - 1] = ARR[start_window + w - 1];
        for (int indice = start_window + w - 2; indice >= start_window; indice--) {
            min_right[indice - start_window] = std::min(min_right[indice - start_window + 1], ARR[indice]);
        }

        for (int j = 0; j < w; j++) {
            sliding_min[start_window + j] = std::min(min_right[j], min_left);
            min_left = (j == 0) ? ARR[start_window + w + j] : std::min(min_left, ARR[start_window + w + j]);
        }
    }

    // last window
    int we_go_up_to = ARR.size() % w + 1;

    int start_window = (nbWin - 1) * w;
    min_right[w - 1] = ARR[start_window + w - 1];
    for (int indice = start_window + w - 2; indice >= start_window; indice--) {
        min_right[indice - start_window] = std::min(min_right[indice - start_window + 1], ARR[indice]);
    }

    for (int j = 0; j < we_go_up_to - 1; j++) {
        sliding_min[start_window + j] = std::min(min_right[j], min_left);
        min_left = (j == 0) ? ARR[start_window + w + j] : std::min(min_left, ARR[start_window + w + j]);
    }

    sliding_min[start_window + we_go_up_to - 1] = std::min(min_right[we_go_up_to - 1], min_left);

    return sliding_min;
}

template <typename T>
inline int doQuery(const T& amqc, const std::string& kmer, bool canonical) {
    // TODO canonical
    return amqc.get(kmer);
}

// TODO even more tests
// OPTIMIZE do min in place (expected gain: *2 speed ratio ?)
/** // TODO redo description
 * @brief Query using findere.
 * @param filterOrTruth the amq wrapped within a customAMQ
 * @param s the sequence to be queried
 * @param K the value for (big) k
 * @param z the current position being queried in s
 * @param canonical do we query canonical kmers ?
 * @return The result of findere's query on s.
 */
template <typename T>
inline std::vector<int> finderec(const T& amqc, const std::string& query, const unsigned int& K, const unsigned int& z, const bool& canonical) {
    if (query.length() < K) {
        throw std::range_error("`query.length()` (which is " + std::to_string(query.length()) + ") < `K` (which is " + std::to_string(K) + ")");
    }

    const unsigned int k = K - z;
    unsigned long long size = query.size();
    std::vector<int> response(size - K + 1, 0);
    unsigned long long stretchLength = 0;  // number of consecutive positives kmers
    unsigned long long j = 0;              // index of the query vector
    bool extending_stretch = true;
    int thisAnswer = 0;
    std::vector<int> previous_answers;
    previous_answers.reserve(response.size());
    while (j < size - k + 1) {
        assert(stretchLength == previous_answers.size());
        if (thisAnswer = doQuery(amqc, query.substr(j, k), canonical)) {
            if (extending_stretch) {
                previous_answers.push_back(thisAnswer);
                stretchLength++;
                j++;
            } else {
                extending_stretch = true;
                j = j - z;
            }
        } else {
            if (stretchLength >= z) {
                unsigned long long t = j - stretchLength;
                for (const auto& minimum : sliding_window_minimum(previous_answers, z + 1)) {
                    response[t++] = minimum;
                }
                // int i = 0;
                // for (unsigned long long t = j - stretchLength; t < j - z; t++) {
                //     response[t] = minElemInWindow(previous_answers, i, i + z + 1);
                //     i++;
                // }
            }
            previous_answers.clear();
            stretchLength = 0;
            extending_stretch = false;
            j = j + z + 1;
        }
    }
    // Last values:
    if (stretchLength >= z) {
        std::vector<int> minimums = sliding_window_minimum(previous_answers, z + 1);
        unsigned long long t = size - k + 1 - stretchLength;
        for (const auto& minimum : sliding_window_minimum(previous_answers, z + 1)) {
            response[t++] = minimum;
        }
        // for (unsigned long long t = size - k + 1 - stretchLength; t < size - K + 1; t++) {
        //     response[t] = minElemInWindow(previous_answers, i, i + z + 1);
        //     i++;
        // }
        // previous_answers.clear();  //optional
    }

    return response;
}