# fimpera <!-- omit in toc -->

[![cpp-app](https://github.com/lrobidou/fimpera/actions/workflows/ci.yml/badge.svg)](https://github.com/lrobidou/fimpera/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/lrobidou/fimpera/branch/master/graph/badge.svg?token=CXO15KTTYE)](https://codecov.io/gh/lrobidou/fimpera)

[![License](http://img.shields.io/:license-affero-blue.svg)](http://www.gnu.org/licenses/agpl-3.0.en.html)


`fimpera` is a simple strategy for speeding up queries and for reducing false positive calls from any Approximate Membership Query data structure (AMQ) that supports abundance queries. <!-- With no drawbacks (in particular no false positive), queries are two times faster with two orders of magnitudes less false positive calls. (TODO: check this statement) --> 

<!-- TODO image -->

The `fimpera` implementation proposed here uses a counting Bloom filter. It proposes a way to index and query Kmers from biological sequences (fastq or fasta, gzipped or not). <!-- , possibly considering only canonical Kmers). (TODO: code and check)--> <!-- or from any textual data (TODO code and check) -->

`fimpera` relies on templates, hence it can be easily adapted to any other AMQ that supports abundance queries, for any usage.

# Table of content <!-- omit in toc -->

- [Install](#install)
  - [Dependencies](#dependencies)
  - [Install `fimpera`](#install-fimpera)
- [Running](#running)
  - [Overview](#overview)
- [Contacts](#contacts)
- [Work in progress](#work-in-progress)

# Install 
## Dependencies

You must first install zlib. It is likely to be already installed, if not you can try:
```bash
sudo apt update
sudo apt-cache search zlib  # zlib1g-dev on Ubuntu 20.04.
sudo apt-get install zlib  # or wathever you found with apt-cache search
```


## Install `fimpera`

```bash
git clone --recursive https://github.com/lrobidou/fimpera
cd fimpera
./install.sh
```

<!-- ## Reproduce paper results -->
<!-- TODO -->
# Running
**note:**  
`fimpera` needs a file containing the abundance of each Kmers to index them (each line should contain a Kmer (not a kmer), one tab and the abundance associated with that Kmer). KMC can provide such file, however you are free to use another kmer counter program.
## Overview

```bash
# indexing a file
./bin/fimpera_index <KMCfile (.txt)> <index_name> [ -b <number of bits per buckets in the filter> -K <K> -z <z> --canonical ]

# querying a file
./bin/fimpera_query <index_name> <your query file>
```
<!-- TODO [ <threshold> ] -->

<!-- ## Example on natural text file
TODO -->

<!-- ## Usage on genomic data (fastq or fasta file)
TODO -->


<!-- # Using fimpera with a personal Approximate Membership Query data structure -->

<!-- TODO -->

# Contacts

Lucas Robidou: lucas.robidou@inria.fr

Pierre Peterlongo: pierre.peterlongo@inria.fr

<!-- # Citation -->

<!-- TODO -->





<!-- ## Reproduce paper results -->
<!-- TODO -->


# Work in progress

`fimpera itself`
- [ ] use uuid to decide whether the index is actually a `fimpera` index
- [ ] read lowercase letters as uppercase letters when indexing genomic data
- [ ] handle 'n' in canonical queries
- [ ] verify that the index exists before trying to query it
- [ ] remove assert in code, replace with exception where needed
- [ ] optimize canonical function


`documentation`
- [ ] (re)write documentation for every function
- [ ] write at least a README.md that explains the way the code is organized


