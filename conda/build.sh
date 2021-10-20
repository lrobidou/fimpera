#!/bin/bash

mkdir build-conda
cd build-conda
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
cd ..

mkdir -p $PREFIX/bin

cp -r ./bin/cpp-app $PREFIX/bin #TODO
