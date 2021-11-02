FROM ubuntu:latest
MAINTAINER Lucas Robidou lucas.robidou@inria.fr

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y dist-upgrade \
    && apt-get install -y --no-install-recommends && apt-get clean

RUN apt-get install -y git cmake gcc g++

RUN cd /opt \
    && git clone --recursive https://github.com/lrobidou/fimpera \
    && cd kmdiff \
    && mkdir build \
    && cd build \
    && cmake .. -DMAX_K=64 -DWITH_MODULES=ON -DWITH_HOWDE=ON -DWITH_SOCKS=ON \
    && make -j8

RUN cd /opt/fimpera && chmod +x ./bin/*

WORKDIR /tmp

ENTRYPOINT ["/opt/fimpera/bin/fimpera"]
