# fimpera-lib headers

In this folder are stored both headers and templates of fimpera.

Among those headers, some are of interest:

### fimpera.hpp
fimpera is the main class of this program. It is a template class that instantiates a filter (any filter can be instantiated as long as its class exposes some required methods, described in the CBF section). It publicly exposes the following methods:

`fimpera(strategyWrapper, filename, K, z, canonical, nbBits, nbBuckets = 1)`
This constructor instantiates an empty filter (the type of the filter must be provided via the C++ template mechanism). It then fills that filter with data from the file named `filename`. The file must consists of a KMC-like output file: each line contains
- a kmer
- a tabulation
- a positive integer

`save(filename)`
This method saves the fimpera object on a disk. The object that was just saved can still be used afterward, as it remains valid.

`fimpera(filename)`
This contructor loads fimpera from a file. Typical use case is:
- call the constructor from a KMC file
- save fimpera on disk for later use
- load it back from disk at query time
- query fimpera
  
<strong>Warning</strong>: for technical reasons, fimepra does not store any information about the type of the filter on disk. It is your responsibility to ensure that the type of the filter on disk is the same as the type passed as template when loading the index from disk. 
Correct exemple:
```c++
    fimpera<countingBF::CBF> original = fimpera<countingBF::CBF>("KMC.txt", 35, 3, false, 10000, 5);
    f1.save(filenameToSaveTo);
    fimpera<countingBF::CBF> copy = fimpera<countingBF::CBF>(filenameToSaveTo);
```
Wrong exemple:
```c++
    fimpera<countingBF::CBF> original = fimpera<countingBF::CBF>("KMC.txt", 35, 3, false, 10000, 5);
    f1.save(filenameToSaveTo);
    fimpera<anyOtherTypeOfFilter> copy = fimpera<anyOtherTypeOfFilter>(filenameToSaveTo);
```
This will likely ends in a crash, and even if it does not, queries made against fimpera will have a random behavior. 

`query(filename, response)`
Performs a query using fimpera. For each queried read, fimpera calls the method `processResult` of the `response` object. See section CustomResponse below.

`getCanonical(), getK(), getz(), getStrategyName()`
All those are getters whose names are self-explanatory. They are mainly designed to retrieve parameters used to instantiate a filter that was loaded from disk.

Other methods are mainly for debug purposes:

`queryRead(read)`
Performs a query on a single read.

`getInnerFilter()`
Retrieves the inner filter of fimpera. Used for unit tests, you should not have to use it.

`operator==(other)`
Compares two fimpera instances.

### CBF.hpp
CBF (for counting Bloom filter) is a class implementing a counting Bloom filter.

Its public fonctions are descibed below. Some of them are required to make a filter works with fimpera, so if you want to use fimpera on your own AMQ, it needs to exposes them as well (or else you will have to change fimpera's code).

[required] `CBF(nbBits = 100, nbBitsPerCell = 1, nbHashFunctions = 1)`
Contructs an empty counting Bloom filter given:
- a total size
- the width of each cell (i.e. if nbBitsPerCell is 10 then the maximal number that can be put in CBF is 2**10 - 1)
- the number of hash function (fimpera is target for applications where the number of hash function is 1, but one can pass any number of hash functions)

[required] `dump(ofstream)`
Dumps the CBF in a file (stream). Fimpera calls that method when saving itself in a file.

[required] `CBF(ifstream)`
Reloads a CBF from a file (stream). Typically called to get back a CBF saved on disk whith the dump method.

[required] `set(kmer, occurrence = 1)`
Store a kmer to a number of occurence (= abundance). Note that CBF is agnostic of fimpera, so from CBF perspetive, it stores kmers.
If a colliding kmer has a higher abundance, this function has no effect.
If a colliding kmer has a lower abundance, this function will replace the abundance of the collinding kmer, potentially leading to an overestimation at query time.

[required] `get(kmer)`
Retrive the abundance of a kmer, potentially an overestimation.

`operator==(other)`
Compares two CBF.

`getStats()`
Returns an histogram of the usage of the filter. Used for debug purposes. As fimpera does not rely on that function, it can be omitted.

### CustomResponse
CustomResponse is a class from which one can inherit. See ResultGetter class for an exemple of how to use it.

[required] `processResult(query_result, K, current_header,current_read)`
each time fimepra gets the result of a query on a read, it calls processResult and passes it:
- the abundance of each kmer
- the value of k
- the header of the current read
- the read itself.
From that, it is easy to implement a class inheriting from CustomResponse that:
- stores every response from every read in a vector (e.g. ResultGetter) for further analysis
- or print them on the fly
- or perform any other function 