#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <memory>
#include <string>

#include "args.hpp"

/*
    Input: a kmc file
    Output: a histogram of the kmers, in a file.

    Histogram looks like:
    abundance: number of time it was seen

    (abundances are sorted)

*/
inline void add(robin_hood::unordered_map<uint64_t, uint64_t>& map, uint64_t key, uint64_t valueToAdd) {
    robin_hood::unordered_map<uint64_t, uint64_t>::const_iterator got = map.find(key);
    if (got != map.end()) {                   // found
        uint64_t val = got->second;           // sauvegarde de la valeur
        map.erase(key);                       // suppression de la valeur dans la map
        map.insert({key, val + valueToAdd});  // insertion de la nouvelle valeur
    } else {                                  // not found
        map.insert({key, valueToAdd});
    }
}

void inline toFileTXT(std::string outfilename, robin_hood::unordered_map<uint64_t, uint64_t> map) {
    remove(outfilename.c_str());

    // get sorted keys
    std::vector<int> keys;
    for (auto const& [key, b] : map) {
        keys.push_back(key);
    }
    sort(keys.begin(), keys.end());

    // print keys and values to file
    std::ofstream outFile(outfilename);
    for (const auto& key : keys) {
        outFile << key << " " << map[key] << "\n";
    }
}

inline void histogram_abundance(const std::string& filename, const std::string& outfilename) {
    robin_hood::unordered_map<uint64_t, uint64_t> histogram{};

    if (!fileExists(filename)) {
        std::string msg = "The file " + filename + " does not exist.";
        throw std::runtime_error(msg);
    }
    std::ifstream myFileGz(filename);
    zstr::istream myFile(myFileGz);

    std::string line;
    std::string kmer;
    while (std::getline(myFile, line)) {  // \n are removed
        std::string Kmer;
        std::string abundanceStr;
        std::stringstream linestream(line);
        std::getline(linestream, Kmer, '\t');
        std::getline(linestream, abundanceStr, '\t');
        add(histogram, std::stoll(abundanceStr), 1);
    }
    toFileTXT(outfilename, histogram);
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_abundance", "0.0.1");
    program.add_argument("kmc_filename").help("kmc file that contains the truth for the Kmers");
    parse(program, argc, argv);
    const std::string kmc_filename = program.get("kmc_filename");
    histogram_abundance(kmc_filename, "histogram_of_true_kmers_from.txt");
}
