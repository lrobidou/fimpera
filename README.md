# fimpera <!-- omit in toc -->

[![cpp-app](https://github.com/lrobidou/fimpera/actions/workflows/ci.yml/badge.svg)](https://github.com/lrobidou/fimpera/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/lrobidou/fimpera/branch/master/graph/badge.svg?token=CXO15KTTYE)](https://codecov.io/gh/lrobidou/fimpera)

[![License](http://img.shields.io/:license-affero-blue.svg)](http://www.gnu.org/licenses/agpl-3.0.en.html)


`fimpera` is a simple strategy for reducing false positive calls from any Approximate Membership Query data structure (AMQ) that supports abundance queries. <!-- With no drawbacks (in particular no false positive), queries are two times faster with two orders of magnitudes less false positive calls. (TODO: check this statement) --> 

<!-- TODO image -->

The `fimpera` implementation proposed here uses a counting Bloom filter. It proposes a way to index and query Kmers from biological sequences (fastq or fasta, gzipped or not). <!-- , possibly considering only canonical Kmers). (TODO: code and check)--> <!-- or from any textual data (TODO code and check) -->

`fimpera` relies on templates, hence it can be easily adapted to any other AMQ that supports abundance queries, for any usage.

## Table of content <!-- omit in toc -->

- [Install](#install)
  - [Dependencies](#dependencies)
  - [Install `fimpera`](#install-fimpera)
- [Running](#running)
  - [Overview](#overview)
- [Contacts](#contacts)
- [Work in progress](#work-in-progress)

## Install 
### Dependencies

You must first install zlib. It is likely to be already installed, if not you can try:
```bash
sudo apt update
sudo apt-cache search zlib  # zlib1g-dev on Ubuntu 20.04.
sudo apt-get install zlib  # or wathever you found with apt-cache search
```


### Install `fimpera`

```bash
git clone --recursive https://github.com/lrobidou/fimpera
cd fimpera
./install.sh
```

<!-- ## Reproduce paper results -->
<!-- TODO -->
## Running
**note:**  
`fimpera` needs a file containing the abundance of each Kmers to index them (each line should contain a Kmer (not a kmer), one tab and the abundance associated with that Kmer). KMC can provide such file, however you are free to use another kmer counter program.
### Overview

```bash
# indexing a file
./bin/fimpera_index <KMCfile (.txt)> <index_name> [ -b <number of bits per buckets in the filter> -K <K> -z <z> --canonical ]

# querying a file
./bin/fimpera_query <index_name> <your query file>
```


## Usage on genomic data (fastq or fasta file)
After installing `fimpera`, you can test it using test samples data file.

Index a simple KMC output file, using 100000 bits, kmer of size 35, 5 encoding bits in the counting bloom filter and z=3.
```
./bin/fimpera_index tests/unit/data/1000LinesTest.txt index.idx 100000 -K 35 -b 5 -z 3
```
```
./bin/fimpera_query index.idx tests/unit/data/exemple_query.fasta                     
```
displays the result of queries against every reads:
```
>genome.1 CRG080910-3E6:8:1:522:318 length=70
T A C A A T G A A G A A C T C A A T C G T A T G C C G T C T T T T G T T A A T G A A G A A C T C A A T C A T C G T C G C C G T C T T T T G T T 
0 0 0 0 0 0 1 1 1 1 1 1 1 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 0 1 0 0 
>genome.2 CRG080910-3E6:8:1:351:650 length=70
G A C A T C A C A T G C T G G G T C G T A T G C C G T C T T T T T C T T A A T G A A G A A C T C A A T C T C G T A T G C C G T C T T T T G T T 
1 0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0 0 
>genome.3 CRG080910-3E6:8:1:754:111 length=70
T  T  A  C  T  C  T  T  T  A  A  A  G  A  G  C  T  G  G  A  A  C  G  T  G  A  A  A  A  T  C  G  T  G  A  A  C  T  G  C  G  C  C  G  A  A  A  G  A  G  C  T  G  G  A  A  C  G  T  G  A  A  A  A  T  C  G  T  G  A  A  
1  1  1  1  1  0  0  0  0  13 1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1  
>genome.4 CRG080910-3E6:8:1:804:412 length=70
T  T  C  T  G  G  T  G  T  T  C  A  C  T  G  T  T  T  C  T  T  T  T  G  C  C  G  T  A  A  T  G  A  A  G  A  A  C  T  C  A  A  T  C  T  T  A  A  T  G  A  A  G  A  A  C  T  C  A  A  T  C  C  A  C  G  G  G  A  T  
1  1  1  0  0  0  0  0  0  0  0  0  1  1  1  1  0  0  0  0  0  0  -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 0  0  0  
>genome.5 CRG080910-3E6:8:1:847:96 length=70
T  A  C  A  C  C  G  G  A  C  A  C  G  G  G  A  T  T  G  G  A  T  G  C  C  C  T  C  T  T  T  T  T  G  T  T  G  G  A  C  A  C  G  G  G  A  T  T  G  G  A  T  T  G  G  A  T  T  G  G  A  T  T  G  G  A  T  T  G  G  
0  0  0  1  0  0  0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1  0  0  -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 
```
under each letter (base), the abundance of the kmer starting from this position is displayed. The abundance is displayed as -1 if the complexity of the kmer is too low (see paper).

# Using fimpera with a personal counting Approximate Membership Query data structure
Explanation of fimpera headers can be found [here](lib/include/fimpera-lib/README.md)
Explanation on how to use another AMQ is [here](lib/include/fimpera-lib/README.md#cbfhpp)

## Contacts

Lucas Robidou: lucas.robidou@inria.fr

Pierre Peterlongo: pierre.peterlongo@inria.fr

<!-- # Citation -->

<!-- TODO -->





<!-- ## Reproduce paper results -->
<!-- TODO -->


## Work in progress

`fimpera itself`
- [ ] use uuid to decide whether the index is actually a `fimpera` index
- [ ] read lowercase letters as uppercase letters when indexing genomic data
- [ ] handle 'n' in canonical queries
`documentation`
- [ ] (re)write documentation for every function
- [ ] write at least a README.md that explains the way the code is organized


