#pragma once

#include <assert.h>

#include <algorithm>
#include <iostream>
#include <string>

inline void reverseComplement(std::string& seq) {
    std::string reverseComplement(seq.rbegin(), seq.rend());
    int l = seq.length();
    bool forward_stricly_higher = false;
    for (int i = 0; i < l; i++) {
        // breaks are unnecessary
        // but they may prevent bugs if someone tries to modify the code
        switch (reverseComplement[i]) {
            case 'A':
                reverseComplement[i] = 'T';
                break;
            case 'T':
                reverseComplement[i] = 'A';
                break;
            case 'G':
                reverseComplement[i] = 'C';
                break;
            case 'C':
                reverseComplement[i] = 'G';
                break;
            case 'a':
                reverseComplement[i] = 't';
                break;
            case 't':
                reverseComplement[i] = 'a';
                break;
            case 'g':
                reverseComplement[i] = 'c';
                break;
            case 'c':
                reverseComplement[i] = 'g';
                break;
            case 'N':
                reverseComplement[i] = 'N';
                break;
            case 'n':
                reverseComplement[i] = 'n';
                break;
            default:
                std::cerr << reverseComplement[i] << "encountered in a kmer while trying to complement it. this is not allowed. Exiting." << std::endl;
                exit(1);
                break;
        }
        if (!forward_stricly_higher) {
            if (seq[i] < reverseComplement[i]) {
                // forward is stricly lower, stop here
                return;
            } else if (seq[i] > reverseComplement[i]) {
                forward_stricly_higher = true;
            }
        }
    }
    // std::cout << "complement" << std::endl;
    seq = reverseComplement;
}

inline std::string toCanonical(const std::string& kmer) {
    // std::string reversed = kmer;
    // reverseComplement(reversed);
    // bool forward_stricly_higher = false;

    // std::size_t s = kmer.size();
    // for (std::size_t i = 0; i < s; i++) {
    //     if (kmer[i] > reversed[i]) {
    //         forward_stricly_higher = true;
    //         break;
    //     } else if (kmer[i] < reversed[i]) {
    //         break;
    //     }
    //     // else, the two letters are equal, let's continue
    // }

    // if (forward_stricly_higher) {
    //     return reversed;
    // }
    // return kmer;
    auto copy = kmer;
    reverseComplement(copy);
    return copy;
}

// inline std::string make_canonical(std::string kmer) {
//     std::string reverseComplement(kmer.rbegin(), kmer.rend());
//     int l = kmer.length();
//     for (int i = 0; i < l; i++) {
//         switch (reverseComplement[i]) {
//             case 'A':
//                 reverseComplement[i] = 'T';
//                 break;
//             case 'C':
//                 reverseComplement[i] = 'G';
//                 break;
//             case 'T':
//                 reverseComplement[i] = 'A';
//                 break;
//             case 'G':
//                 reverseComplement[i] = 'C';
//                 break;
//             case 'N':
//                 reverseComplement[i] = 'N';
//                 break;
//             default:
//                 std::cerr << reverseComplement[i] << "encountered in a kmer. this is not allowed. Exiting." << std::endl;
//                 exit(1);
//                 break;
//         }
//         if (reverseComplement[i] > kmer[i]) {
//             return kmer;
//         }
//     }
//     return reverseComplement;
// }