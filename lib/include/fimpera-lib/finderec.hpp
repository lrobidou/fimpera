#pragma once
#include <assert.h>  //TODO remove

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

template <typename T>
inline int doQuery(const T& amqc, const std::string& kmer, bool canonical) {
    // TODO canonical
    return amqc.get(kmer);
}

/** // TODO refaire description
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
                int i = 0;
                for (unsigned long long t = j - stretchLength; t < j - z; t++) {
                    // response[t] = previous_answers[i++];
                    response[t] = minElemInWindow(previous_answers, i, i + z + 1);
                    i++;
                }
            }
            previous_answers.clear();
            stretchLength = 0;
            extending_stretch = false;
            j = j + z + 1;
        }
    }
    // Last values:
    // std::cout << "last values" << std::endl;
    if (stretchLength >= z) {
        int i = 0;
        for (unsigned long long t = size - k + 1 - stretchLength; t < size - K + 1; t++) {
            // std::cout << "t = " << t << std::endl;
            // response[t] = previous_answers[i++];
            response[t] = minElemInWindow(previous_answers, i, i + z + 1);
            i++;
        }
        previous_answers.clear();  //optional
    }

    return response;
}