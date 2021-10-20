# cpp-app-template

[![cpp-app](https://github.com/tlemane/cpp-app-template/actions/workflows/ci.yml/badge.svg)](https://github.com/tlemane/cpp-app-template/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/tlemane/cpp-app-template/branch/master/graph/badge.svg?token=CXO15KTTYE)](https://codecov.io/gh/tlemane/cpp-app-template)

A c++ project template.

## Features

### CMake build

```bash
mkdir build && cd build && cmake .. && make
```

* Application binary: `build/app/*`
* Lib : `build/lib/*`
* Configuration header : `build/include/*`

#### Tests


```bash
cmake .. -DWITH_TESTS=ON && make
```
Produces an unique executable for all test suites `tests/unit/test_*.cpp` and add two targets: `test` and `test_memcheck`.

```bash
make test && make test_memcheck
```

#### Coverage

Requirements:
* gcov
* lcov
* genhtml

```bash
cmake .. -DCMAKE_BUILD_TYPE=Coverage -DWITH_TESTS=ON -DWITH_COVERAGE=ON && make && make coverage-report
```
Produces an html report at `<build_dir>/coverage-reports`.

#### Profiling

Requirements:
* valgrind
* valgrind development files

```bash
cmake .. -DCMAKE_BUILD_TYPE=Profile -DWITH_PROFILE=ON && make
```
Produces a profilable executable for each `tests/profiles/profile_*.cpp` at `<build_dir>/profile_bin` and adds the corresponding targets `profile-profile_*`.

```bash
make profile-profile_ex
```
Produces `profile-profile_ex.log` and `profile-profile_ex.out` at `<build_dir>/profiles`.

#### CppCheck

Requirements:
* cppcheck
* cppcheck-htmlreport

```bash
cmake .. -DWITH_CPPCHECK=ON && make && make cppcheck-report
```
Produces an html report at `<build_dir>/cppcheck-report`.
#### Doxygen

Requirements:
* doxygen

Configuration: `Doxyfile`

```bash
cmake .. -DWITH_DOC=ON && make && make documentation
```
Produces html documentation at `<build_dir>/documentation`.

#### ClangFormat

Requirements:
* clang-format

Configuration: `.clang-format`

```bash
cmake .. -DWITH_CLANGFORMAT=ON
make format-lib # format file at libs/
make format-app # format file at app/
```

#### Packaging

Configuration: `cmake/CPackConfig.cmake`

```bash
cmake .. && make && make package
```
Produces an archive `<build_dir>/<app_name>-vX.Y.Z-bin-<system>.tar.gz` with:
* `bin/<app_name>`
* `lib/<lib_name>.a|so>`
* `include/<lib_name>/*.hpp`

### Github Action CI

#### Matrix build

* os: Ubuntu-20.04, MacOS 10.15
* compiler (Linux only): gcc8, gcc9, gcc10

#### Code coverage

* Automatic report on [CodeCov](https://app.codecov.io/gh/tlemane/cpp-app-template/).
* Requires to add `CODECOV_TOKEN` in repository's secrets.

#### Automatic release

1. Create a changelog file


```
> cat docs/changelogs/vX.Y.Z.md
### cpp-app-template vX.Y.Z
* Auto release example
```

2. Add a new tag

```
git tag vX.Y.Z
git push origin vX.Y.Z
```

A new release is then created by Github Actions with the content of `docs/changelogs/vX.Y.Z.md` and the following assets:
  * `<name>-vX.Y.Z-bin-Linux.tar.gz` (compiled binary for Linux)
  * `<name>-vX.Y.Z-bin-Darwin.tar.gz` (compiled binary for MacOS)
  * `<name>-vX.Y.Z-sources.tar.gz` (sources including submodules)