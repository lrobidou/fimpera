#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

class ResultGetter : public CustomResponse {
   private:
    std::vector<int> entireResponse;

   public:
    ResultGetter() {}

    void
    processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
        entireResponse.insert(std::end(entireResponse), std::begin(res), std::end(res));
    }

    std::vector<int> getResult() {
        return entireResponse;
    }
};

class TruthInTheShapeOfAnAMQ {
   private:
    uint64_t _nbBuckets;
    uint64_t _nbCells;
    uint64_t _limitValueInBucket;
    robin_hood::unordered_map<std::string, int> _t;

   public:
    TruthInTheShapeOfAnAMQ(int nbBits, int nbBuckets) : _nbBuckets(nbBuckets) {
        _nbCells = nbBits / _nbBuckets;
        _limitValueInBucket = pow(2, _nbBuckets) - 1;
    }

    bool set(const std::string& x, int occurrence = 1) {
        // get existing data
        int data = get(x);

        // compute new value of data
        if (data < occurrence) {
            data = occurrence;
            // TODO discuss with Pierre
            // if (data > _limitValueInBucket) {
            //     data = _limitValueInBucket;
            // }
        }

        auto it = _t.find(x);
        if (it != _t.end()) {
            it->second = data;
        } else {
            _t.insert({x, data});
        }
        return data;
    }

    int get(const std::string& x) const {
        auto it = _t.find(x);
        if (it != _t.end()) {
            return it->second;
        } else {
            return 0;
        }
    }
};

inline void add(robin_hood::unordered_map<int, int>& map, const int& key, const int& valueToAdd) {
    robin_hood::unordered_map<int, int>::const_iterator got = map.find(key);
    if (got != map.end()) {                   // found
        int val = got->second;                // sauvegarde de la valeur
        map.erase(key);                       // suppression de la valeur dans la map
        map.insert({key, val + valueToAdd});  // insertion de la nouvelle valeur
    } else {                                  // not found
        map.insert({key, valueToAdd});
    }
}

inline int absDiff(const int& a, const int& b) {
    int abs_diff = (a > b) ? (a - b) : (b - a);
    return abs_diff;
}

inline robin_hood::unordered_map<int, int> getHistogram(std::vector<int> a, std::vector<int> b) {
    assert(a.size() == b.size());
    int n = a.size();
    robin_hood::unordered_map<int, int> histogram;
    for (int i = 0; i < n; ++i) {
        add(histogram, absDiff(a[i], b[i]), 1);
    }
    return histogram;
}

void inline toFileTXT(std::string outfilename, robin_hood::unordered_map<int, int> map) {
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

std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> compareVectors(const std::vector<int>& result, const std::vector<int>& truth) {
    std::size_t tp = 0;
    std::size_t tn = 0;
    std::size_t fp = 0;
    std::size_t fn = 0;

    assert(result.size() == truth.size());

    for (std::size_t i = 0; i < result.size(); i++) {
        if (truth[i]) {
            if (result[i]) {
                tp++;
            } else {
                fn++;
            }
        } else {
            if (result[i]) {
                fp++;
            } else {
                tn++;
            }
        }
    }
    return {tp, tn, fp, fn};
}

void writeConstructionFPToFile(const fimpera<countingBF::CBF>& index, const std::string& queryFile, const std::vector<int>& construction_truth_response, const std::vector<int>& truth_response, const std::string& outfilename) {
    remove(outfilename.c_str());

    // print keys and values to file
    std::ofstream outFile(outfilename);

    FileManager reader = FileManager();
    reader.addFile(queryFile);
    std::string current_read;

    int pos = 0;

    while (!(current_read = reader.get_next_read()).empty()) {
        std::size_t size = current_read.length();
        std::size_t j = 0;  // start of the kmer in the Kmer
        while (j < size - (index.getK() - index.getz()) + 1) {
            if (construction_truth_response[pos] && !truth_response[pos]) {
                outFile << current_read.substr(j, index.getK() - index.getz()) << "\n";
            }
            pos++;
            j++;
        }
    }
}

template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto since(std::chrono::time_point<clock_t, duration_t> const& start) {
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

// inline std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> queryLowMemory(fimpera<TruthInTheShapeOfAnAMQ>& truth, fimpera<countingBF::CBF>& index, const std::string& filename) {
//     FileManager reader = FileManager();
//     std::size_t tp = 0;
//     std::size_t tn = 0;
//     std::size_t fp = 0;
//     std::size_t fn = 0;

//     for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename)) {
//         std::vector<int> res = index.queryRead(read);
//         std::vector<int> res_truth = truth.queryRead(read);
//         const auto& [tpp, tnp, fpp, fnp] = compareVectors(res, res_truth);

//         tp += tpp;
//         tn += tnp;
//         fp += fpp;
//         fn += fnp;
//     }

//     return {tp, tn, fp, fn};
// }

using stats_t = std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>;
using index_and_truth = std::tuple<const fimpera<countingBF::CBF>&, const fimpera<TruthInTheShapeOfAnAMQ>&>;

namespace internal {
// see: https://stackoverflow.com/a/16387374/4181011
template <typename T, size_t... Is>
void add_rhs_to_lhs(T& t1, const T& t2, std::integer_sequence<size_t, Is...>) {
    auto l = {(std::get<Is>(t1) += std::get<Is>(t2), 0)...};
    (void)l;  // prevent unused warning
}
}  // namespace internal

template <typename... T>
std::tuple<T...>& operator+=(std::tuple<T...>& lhs, const std::tuple<T...>& rhs) {
    internal::add_rhs_to_lhs(lhs, rhs, std::index_sequence_for<T...>{});
    return lhs;
}

template <typename... T>
std::tuple<T...> operator+(std::tuple<T...> lhs, const std::tuple<T...>& rhs) {
    return lhs += rhs;
}

inline std::vector<std::tuple<stats_t, stats_t>> queryLowMemory(const fimpera<TruthInTheShapeOfAnAMQ>& truth, const std::vector<index_and_truth>& indexes_and_ctruths, const std::string& filename) {
    FileManager reader = FileManager();

    // std::size_t tp = 0;
    // std::size_t tn = 0;
    // std::size_t fp = 0;
    // std::size_t fn = 0;
    // std::valarray<std::size_t>{0, 0, 0, 0};
    std::vector<std::tuple<stats_t, stats_t>> stats_truth;
    // std::vector<stats> stats_ctruth;

    std::size_t size = indexes_and_ctruths.size();

    stats_truth.reserve(size);
    // stats_ctruth.reserve(size);

    for (size_t i = 0; i < size; i++) {
        stats_truth.push_back({{0, 0, 0, 0}, {0, 0, 0, 0}});
        // stats_ctruth.push_back({0, 0, 0, 0});
    }

    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename)) {
        std::vector<int> res_truth = truth.queryRead(read);

        std::size_t i = 0;
        for (const auto& [index, ctruth] : indexes_and_ctruths) {
            std::get<0>(stats_truth[i]) += compareVectors(index.queryRead(read), res_truth);
            std::get<1>(stats_truth[i]) += compareVectors(ctruth.queryRead(read), res_truth);
            i++;
        }
    }

    return stats_truth;
}

// using iso_b_t = fimpera<countingBF::CBF>;
// using iso_size_t = std::tuple<std::size_t, iso_b_t>;                                             // b, iso_b
// using iso_z_t = std::tuple<std::size_t, std::vector<iso_size_t>>;                                // size, iso_size
// using iso_k_t = std::tuple<std::size_t, fimpera<TruthInTheShapeOfAnAMQ>, std::vector<iso_z_t>>;  // z, ctruth, iso_z
// using input_t = std::vector<std::tuple<std::size_t, fimpera<TruthInTheShapeOfAnAMQ>, std::vector<iso_k_t>>>;

// void f(const input_t& input) {
//     // assumes all vector of input have the same size, ie: "parallélotope droit" in french
//     for (const auto& [k, truth_index, iso_k_vector] : input) {
//         for (const auto& [z, ctruth_index, iso_z_vector] : iso_k_vector) {
//             for (const auto& [size, iso_size_vector] : iso_z_vector) {
//                 for (const auto& [b, filter] : iso_size_vector) {
//                     // for (const auto& filter : iso_b_vector) {
//                     //     //
//                     // }
//                 }
//             }
//         }
//     }
// }

// input_t getMatrix(const std::string& indexBaseFilename,
//                   const std::string& KMCFilename,
//                   const std::string& queryFile,
//                   const std::vector<std::size_t>& ks,
//                   const std::vector<std::size_t>& zs,
//                   const std::vector<std::size_t>& sizes,
//                   const std::vector<std::size_t>& bs) {
//     std::vector<std::tuple<std::size_t, fimpera<TruthInTheShapeOfAnAMQ>, std::vector<iso_k_t>>> input;
//     for (const auto& k : ks) {
//         std::tuple<std::size_t, fimpera<TruthInTheShapeOfAnAMQ>, std::vector<iso_k_t>> tuple = std::make_tuple(k, );
//         std::vector<iso_k_t> vector;
//         for (const auto& z : zs) {
//             std::string ctruthFileName = "";
//             iso_k_t iso_k = std::make_tuple(z, fimpera<TruthInTheShapeOfAnAMQ>(ctruthFileName), std::vector<iso_z_t>());
//             for (const auto& size : sizes) {
//                 iso_z_t iso_z = std::make_tuple(size, std::vector<iso_size_t>());
//                 for (const auto b : bs) {
//                     std::string indexFileName = "a";  // TODO remplir et deplacer ?
//                     std::get<1>(iso_z).push_back(std::make_tuple(b, fimpera<countingBF::CBF>(indexFileName)));
//                 }
//                 // iso_z ready
//                 std::get<2>(iso_k).push_back(iso_z);
//             }
//             // iso_k ready
//             vector.push_back(iso_k);
//         }
//         // vector ready
//         input.push_back(std::make_tuple(k, fimpera<TruthInTheShapeOfAnAMQ>("TODO"), vector));
//     }
//     return input;

//     // for (const auto& [k, truth_index, iso_k_vector] : input) {
//     //     for (const auto& [z, ctruth_index, iso_z_vector] : iso_k_vector) {
//     //         for (const auto& [size, iso_size_vector] : iso_z_vector) {
//     //             for (const auto& [b, filter] : iso_size_vector) {
//     //                 // for (const auto& filter : iso_b_vector) {
//     //                 //     //
//     //                 // }
//     //             }
//     //         }
//     //     }
//     // }
// }
using output_iso_b_t = std::tuple<std::size_t, stats_t>;                                  // b, iso_b
using output_iso_size_t = std::tuple<std::size_t, std::vector<output_iso_b_t>>;           // size, iso_size
using output_iso_z_t = std::tuple<std::size_t, stats_t, std::vector<output_iso_size_t>>;  // z, ctruth, iso_z
using output_iso_k_t = std::tuple<std::size_t, std::vector<output_iso_z_t>>;              // k, ~~truth~~, iso_k
using output_t = std::vector<output_iso_k_t>;

using input_iso_b_t = std::tuple<std::size_t, fimpera<countingBF::CBF>>;                                        // b, iso_b
using input_iso_size_t = std::tuple<std::size_t, std::vector<input_iso_b_t>>;                                   // size, iso_size
using input_iso_z_t = std::tuple<std::size_t, fimpera<TruthInTheShapeOfAnAMQ>, std::vector<input_iso_size_t>>;  // z, ctruth, iso_z
using input_iso_k_t = std::tuple<std::size_t, fimpera<TruthInTheShapeOfAnAMQ>, std::vector<input_iso_z_t>>;     // k, truth, iso_k
using input_t = std::vector<input_iso_k_t>;

output_t queryMatrix(const std::string& indexBaseFilename,
                     const std::string& KMCFilename,
                     const std::string& queryFileName,
                     const bool canonical,
                     const std::vector<std::size_t>& ks,
                     const std::vector<std::size_t>& zs,
                     const std::vector<std::size_t>& sizes,
                     const std::vector<std::size_t>& bs) {
    input_t input;
    for (const auto& k : ks) {
        std::vector<input_iso_z_t> x;
        for (const auto& z : zs) {
            std::vector<input_iso_size_t> w;
            for (const auto& size : sizes) {
                std::vector<input_iso_b_t> v;
                for (const auto b : bs) {
                    std::string filename = indexBaseFilename +
                                           "_k" +
                                           std::to_string(k) +
                                           "_z" +
                                           std::to_string(z) +
                                           "_s" +
                                           std::to_string(size) +
                                           "_b" +
                                           std::to_string(b) +
                                           ".idx";
                    v.push_back(std::make_tuple(b, fimpera<countingBF::CBF>(filename)));
                }
                w.push_back(std::make_tuple(size, v));
            }
            x.push_back(std::make_tuple(z, fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, k, z, canonical, 1, 1), w));
        }
        input.push_back(std::make_tuple(k, fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, k, 0, canonical, 1, 1), x));
    }

    // output initialised

    output_t output;
    for (const auto& k : ks) {
        std::vector<output_iso_z_t> x;
        for (const auto& z : zs) {
            std::vector<output_iso_size_t> w;
            for (const auto& size : sizes) {
                std::vector<output_iso_b_t> v;
                for (const auto b : bs) {
                    v.push_back(std::make_tuple(b, std::make_tuple(0, 0, 0, 0)));
                }
                w.push_back(std::make_tuple(size, v));
            }
            x.push_back(std::make_tuple(z, std::make_tuple(0, 0, 0, 0), w));
        }
        output.push_back(std::make_tuple(k, x));
    }

    // output initialised

    // using output_iso_b_t = std::tuple<std::size_t, stats_t>;                                  // b, iso_b
    // using output_iso_size_t = std::tuple<std::size_t, std::vector<output_iso_b_t>>;           // size, iso_size
    // using output_iso_z_t = std::tuple<std::size_t, stats_t, std::vector<output_iso_size_t>>;  // z, ctruth, iso_z
    // using output_iso_k_t = std::tuple<std::size_t, stats_t, std::vector<output_iso_z_t>>;     // k, truth, iso_k
    // using output_t = std::vector<output_iso_k_t>;
    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(queryFileName)) {
        for (std::size_t k_i = 0; k_i < ks.size(); k_i++) {
            std::vector<int> res_truth_k;  // TODO = truth.queryRead(read);
            for (std::size_t z_i = 0; z_i < ks.size(); z_i++) {
                for (std::size_t size_i = 0; size_i < ks.size(); size_i++) {
                    for (std::size_t b_i = 0; b_i < ks.size(); b_i++) {
                        stats_t& tuple_stats_index = std::get<1>(std::get<1>(std::get<2>(std::get<1>(output[k_i])[z_i])[size_i])[b_i]);
                        const fimpera<countingBF::CBF>& filter = std::get<1>(std::get<1>(std::get<2>(std::get<2>(input[k_i])[z_i])[size_i])[b_i]);
                        tuple_stats_index += compareVectors(filter.queryRead(read), res_truth_k);
                    }
                }
                stats_t& tuple_stats_ctruth = std::get<1>(std::get<1>(output[k_i])[z_i]);
                const fimpera<TruthInTheShapeOfAnAMQ>& ctruth = std::get<1>(std::get<2>(input[k_i])[z_i]);
                tuple_stats_ctruth += compareVectors(ctruth.queryRead(read), res_truth_k);
            }
            // stats_t& tuple_stats_truth = std::get<1>(output[k_i]);
            // const fimpera<TruthInTheShapeOfAnAMQ>& truth = std::get<1>(input[k_i]);
        }
    }
    return output;

    // for (const auto& [k, truth_index, iso_k_vector] : input) {
    //     for (const auto& [z, ctruth_index, iso_z_vector] : iso_k_vector) {
    //         for (const auto& [size, iso_size_vector] : iso_z_vector) {
    //             for (const auto& [b, filter] : iso_size_vector) {
    //                 // for (const auto& filter : iso_b_vector) {
    //                 //     //
    //                 // }
    //             }
    //         }
    //     }
    // }
}

std::string toString(stats_t x) {
    const auto& [tp, tn, fp, fn] = x;
    return std::to_string(tp) + std::to_string(tn) + std::to_string(fp) + std::to_string(tn);
}

void printResultMatrix(output_t all_queries_reponses) {
    // using output_iso_b_t = std::tuple<std::size_t, stats_t>;                                  // b, iso_b
    // using output_iso_size_t = std::tuple<std::size_t, std::vector<output_iso_b_t>>;           // size, iso_size
    // using output_iso_z_t = std::tuple<std::size_t, stats_t, std::vector<output_iso_size_t>>;  // z, ctruth, iso_z
    // using output_iso_k_t = std::tuple<std::size_t, std::vector<output_iso_z_t>>;              // k, ~~truth~~, iso_k
    // using output_t = std::vector<output_iso_k_t>;

    for (const auto& [k, iso_z_vector] : all_queries_reponses) {
        for (const auto& [z, ctruth_stat, iso_size_vector] : iso_z_vector) {
            for (const auto& [size, iso_b_vector] : iso_size_vector) {
                for (const auto& [b, stats_filter] : iso_b_vector) {
                    std::cout << k << " " << z << " " << toString(ctruth_stat) << " " << size << " " << b << " " << toString(stats_filter) << std::endl;
                }
            }
        }
    }
}
void compareWithTruth(const std::string& indexBaseFilename,
                      const std::string& KMCFilename,
                      const std::string& queryFileName) /*
                       const std::vector<std::size_t>& ks,
                       const std::vector<std::size_t>& zs,
                       const std::vector<std::size_t>& sizes,
                       const std::vector<std::size_t>& bs)*/
{
    const std::vector<std::size_t>& ks = {31};
    const std::vector<std::size_t>& zs = {0, 3, 5};
    const std::vector<std::size_t>& sizes = {11000};
    const std::vector<std::size_t>& bs = {1, 3, 5};
    output_t all_queries_reponses = queryMatrix(indexBaseFilename, KMCFilename, queryFileName, false, ks, zs, sizes, bs);
    printResultMatrix(all_queries_reponses);
    // TODO move to an evaluation module
    // read / create indexes
    /// path/name_k${k}_z${z}_s${space}_b${b}.
    // crete input
    // const auto& [k, truth_index, iso_k_vector] = ks;
    // for (const auto& [z, ctruth_index, iso_z_vector] : iso_k_vector) {
    //     for (const auto& [size, iso_size_vector] : iso_z_vector) {
    //         for (const auto& [b, iso_b_vector] : iso_size_vector) {
    //             for (const auto& filter : iso_b_vector) {
    //             }
    //         }
    //     }
    // }

    // const fimpera<countingBF::CBF> index = fimpera<countingBF::CBF>(indexFilename);
    // std::cout << "index BF in (ms)=" << since(start).count() << std::endl;
    // const fimpera<TruthInTheShapeOfAnAMQ> truth = fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, index.getK(), 0, index.getCanonical(), size, nbBuckets);
    // std::cout << "index truth in (ms)=" << since(start).count() << std::endl;

    // fimpera<TruthInTheShapeOfAnAMQ> constrution_truth = fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, index.getK(), index.getz(), index.getCanonical(), size, nbBuckets);

    // std::vector<index_and_truth> idx{{index, constrution_truth}, {index, constrution_truth}};

    // std::vector<std::tuple<stats, stats>> st = queryLowMemory(truth, idx, queryFile);
    // for (const auto& s : st) {
    //     const auto& [tp, tn, fp, fn] = std::get<0>(s);
    //     std::cout << tp << " " << tn << " " << fp << " " << fn << std::endl;
    //     std::cout << "fpr = " << ((double)fp) / ((double)(fp + tn)) << std::endl;

    //     const auto& [tpc, tnc, fpc, fnd] = std::get<1>(s);
    //     std::cout << tpc << " " << tnc << " " << fpc << " " << fnd << std::endl;
    //     std::cout << "fprc = " << ((double)fpc) / ((double)(fpc + tnc)) << std::endl;
    // }

    // const std::string name = std::to_string(index.getK()) + "_" + std::to_string(index.getz()) + "_" + std::to_string(nbBuckets);

    // toFileTXT("histo_" + name + ".txt", getHistogram(index_response, truth_response));
    // toFileTXT("histo_construction_" + name + ".txt", getHistogram(construction_truth_response, truth_response));
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("input_filename").help("index you want to query");
    program.add_argument("query_filename").help("file you want to query against the index");
    program.add_argument("kmc_filename").help("kmc file that contains the truth for the Kmers");
    // program.add_argument("K").help("size of Kmers").scan<'i', int>();
    // program.add_argument("K").help("kmc file that contains the truth for the Kmers");

    parse(program, argc, argv);
    // optional arguments
    // TODO use value stored in filter
    // program.add_argument("K").help("size of Kmers").scan<'i', int>();
    // program.add_argument("-z").help("value of z (cf paper of findere)").default_value(3).scan<'i', int>();

    const std::string index_filename = program.get("input_filename");
    const std::string query_filename = program.get("query_filename");
    const std::string kmc_filename = program.get("kmc_filename");
    // const int K = program.get<int>("K");

    compareWithTruth(index_filename, kmc_filename, query_filename);  //, 1, 1, K);  // TODO copy data from index ?
}
