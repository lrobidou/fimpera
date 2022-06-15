#pragma once

#include <assert.h>

#include <algorithm>
#include <string>

inline char complement(char n) {
    // breaks are unnecessary
    // but they may prevent bugs if someone tries to modify the code
    switch (n) {
        case 'A':
            return 'T';
            break;
        case 'T':
            return 'A';
            break;
        case 'G':
            return 'C';
            break;
        case 'C':
            return 'G';
            break;
        case 'a':
            return 't';
            break;
        case 't':
            return 'a';
            break;
        case 'g':
            return 'c';
            break;
        case 'c':
            return 'g';
            break;
        case 'N':
            return 'N';
            break;
        case 'n':
            return 'n';
            break;
    }
    assert(false);
    return ' ';
}

inline void reverseComplement(std::string& seq) {
    int l = seq.length();
    for (int i = 0; i < l; i++) {
        seq[i] = complement(seq[i]);
    }
    std::reverse(seq.begin(), seq.end());
}

inline std::string toCanonical(const std::string& kmer) {
    std::string reversed = kmer;
    reverseComplement(reversed);
    bool forward_stricly_higher = false;

    std::size_t s = kmer.size();
    for (std::size_t i = 0; i < s; i++) {
        if (kmer[i] > reversed[i]) {
            forward_stricly_higher = true;
            break;
        } else if (kmer[i] < reversed[i]) {
            break;
        }
        // else, the two letters are equal, let's continue
    }

    if (forward_stricly_higher) {
        return reversed;
    }
    return kmer;
}