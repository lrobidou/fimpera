# Data used to run unit tests

## How to generate them

```bash
# basic index generation
./build/app/fimpera_index tests/unit/data/1000LinesTest.txt tests/unit/data/test1000Lines.idx 10000 -b 5 -K 35 -z 3
# contains data from another file (ie. something else than 1000LinesTest.txt)
./build/app/fimpera_index /path_to_a_file_containing_35mers/data/35merlists.txt tests/unit/data/test.idx 10000 -b 5 -K 35 -z 3
# by default, the strategy applied is to store the raw abundance in the filter, and to not apply any function on it (ie. the class of the abundance is the abundance itself)
./bin/fimpera_index tests/unit/data/1000LinesTest.txt tests/unit/data/index_1000LinesTest_identity_strategy_as_default.idx 10000 -K 35 -b 5 -z 3
```
Modifiy index.cpp to pass idStrategy as first argument of the fimpera constructor.
```bash
install.sh # recompile
# chose explicitly to store the raw abundance in the filter
./bin/fimpera_index tests/unit/data/1000LinesTest.txt tests/unit/data/index_1000LinesTest_identity_strategy_as_default.idx 10000 -K 35 -b 5 -z 3
```

Modifiy index.cpp to pass logStrategy as first argument of the fimpera constructor.
```bash
install.sh # recompile
# tell fimpera to store the log2 of the abundance in the filter (ie. class(abundance) = log2(abundance) )
./bin/fimpera_index tests/unit/data/1000LinesTest.txt tests/unit/data/index_1000LinesTest_log2_strategy.idx 10000 -K 35 -b 5 -z 3
```

Modifiy index.cpp to pass idStrategy as first argument of the fimpera constructor.

In the constructor of fimpera, replace
```cpp
this->_strategy = strategyWrapper.name();
```
by
```cpp
this->_strategy = "log2";
```
to force the strategy name to be invalid.
```bash
install.sh # recompile
# here, fimpera stores the raw abundance, but declares that it stored the log2 of the abundance
# this is a bug
# do NOT do that in real life application
# it is done here to test every line of the program (including security checks)
# you MUST revert those changes after generating the flawed index
./bin/fimpera_index tests/unit/data/1000LinesTest.txt tests/unit/data/index_1000LinesTest_identity_strategy_but_the_inner_name_is_called_log2.idx  10000 -K 35 -b 5 -z 3
```

Repair the constructor you just "destroyed" by undoing your modifications.