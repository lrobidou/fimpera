#include <fimpera-lib/InvalidKmerFilter.hpp>
#include <iostream>  // TODO remove
const char* IsInvalid_init_wasCalledTwice::what() const throw() {
    return "Trying to computer complexity of a kmer using a way too complex function. The class \"InvalidKmerFilter\" is able to remember the previous kmer given. As such, one should call isInvalid_init once and after that call isInvalid. That way, the kmer will not be computed completely again. To prevent anyone from using isInvalid_init more than once, this function will crash.";
}

/*
Filter out low complexity kmer. complexity of a kmer is the number of 5-mer it contains.
This code stores the number of 5-mers in the variable _kmers, that acts as an histogram.
To spare space, kmers are stored in tha binary format (ie: A=0b00, C=0b01, T=0b10, G=0b11)
Every other letter is stored as 0b100.
This is likely to underestimate the complexity of a kmer if it contains not only {A,T,C,G}.
*/
uint16_t InvalidKmerFilter::_fromLetterToBin(char letter) {
    switch (letter) {
        case 'a':
        case 'A':
            return 0;
            break;
        case 'c':
        case 'C':
            return 1;
            break;
        case 't':
        case 'T':
            return 2;
            break;
        case 'g':
        case 'G':
            return 3;
            break;
        default:
            std::cerr << "Invalid letter: " << letter << std::endl;
            return 4;
            break;
    }
}

void InvalidKmerFilter::_addKmerBinToKmersVectAndUpdateNumberOfDistinctKmers(const char letter) {
    _kmers[_kmerBin] -= 1;
    if (_kmers[_kmerBin] == 0) {
        // we just incremented _kmers[_kmerBin]
        // so if it is 0 now, it just went from 1 to 0
        // which means that this distinct kmer disappeared
        _distinctKmers--;
    } else {
        // nothing
    }

    _kmerBin <<= 3;
    _kmerBin += _fromLetterToBin(letter);
    _kmerBin &= _kmerBinMask;

    _kmers[_kmerBin] += 1;
    if (_kmers[_kmerBin] == 1) {
        // we just incremented _kmers[_kmerBin]
        // so if it is 1 now, it just went from 0 to 1
        // which means that it is a new distinct kmer
        _distinctKmers++;
    } else {
        // nothing
    }
}

InvalidKmerFilter::InvalidKmerFilter(int k) : _kmerBin(0), _first(true), _kmers(32769, 0), _smallk(5), _k(k), _distinctKmers(0) {
    _kmerBinMask = 1;
    _kmerBinMask <<= (3 * 5);
    _kmerBinMask -= 1;
    _maxNumberOfSmallKmersInTheKmer = _k - _smallk + 1;
}

bool InvalidKmerFilter::isInvalid_init(const std::string& sequence) {
    if (!(_first)) {
        throw IsInvalid_init_wasCalledTwice();
    }
    int i = 0;
    for (const auto letter : sequence) {
        if (i == 4) {  // 5th iteration
            _kmerBin <<= 3;
            _kmerBin += _fromLetterToBin(letter);
            _kmerBin &= _kmerBinMask;
            _kmers[_kmerBin] += 1;
            if (_kmers[_kmerBin] == 1) {
                _distinctKmers++;
            }

        } else if (i > 4) {
            _addKmerBinToKmersVectAndUpdateNumberOfDistinctKmers(letter);
        }
        _kmerBin <<= 3;
        _kmerBin += _fromLetterToBin(letter);
        i++;
    }
    float complexity = ((float)_distinctKmers) / _maxNumberOfSmallKmersInTheKmer;
    return complexity < 0.75;
}

bool InvalidKmerFilter::isInvalid(const char letter) {
    _addKmerBinToKmersVectAndUpdateNumberOfDistinctKmers(letter);
    float complexity = ((float)_distinctKmers) / _maxNumberOfSmallKmersInTheKmer;
    return complexity < 0.75;
}

float InvalidKmerFilter::getComplexity() {
    float complexity = ((float)_distinctKmers) / ((float)_maxNumberOfSmallKmersInTheKmer);
    return complexity;
}
