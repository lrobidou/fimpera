#include <stdint.h>

#include <exception>
#include <string>
#include <vector>

struct IsInvalid_init_wasCalledTwice : public std::exception {
    const char* what() const throw();
};

class InvalidKmerFilter {
   private:
    // bin of the kmer (replacement of letter from a kmer with _fromLetterToBin)
    uint16_t _kmerBin;
    // mask to get rid of "high" bit above the max len of _kmerBin
    uint16_t _kmerBinMask;
    // is it the first time we call isInvalid with a sequence ?
    bool _first;
    // a vector of kmer presence.
    // Basically like a counting Bloom filter,
    // but with a size chosen high enough to get rid of FP.
    // index of this vector is _kmerBin
    std::vector<uint8_t> _kmers;
    // the value of the small kmer we are counting to estimate the complexity
    // *not* related to k or s from findere
    int _smallk;
    // the lenght of kmer to extract from a sequence
    int _k;
    // the number of distinct small kmer in the kmer
    int _distinctKmers;
    int _maxNumberOfSmallKmersInTheKmer;

    uint16_t _fromLetterToBin(char letter);

    void _addKmerBinToKmersVectAndUpdateNumberOfDistinctKmers(const char letter);

   public:
    InvalidKmerFilter(int k);
    bool isInvalid_init(const std::string& sequence);
    bool isInvalid(const char letter);
    float getComplexity();
};