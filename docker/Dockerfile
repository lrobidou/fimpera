FROM ubuntu:latest
MAINTAINER Téo Lemane teo.lemane@inria.fr #TODO

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y dist-upgrade \
    && apt-get install -y --no-install-recommends && apt-get clean

RUN apt-get install -y git cmake gcc g++

RUN cd /opt \
    && git clone --recursive https://github.com/tlemane/cpp-app-template \ #TODO
    && cd kmdiff \
    && mkdir build \
    && cd build \
    && cmake .. -DMAX_K=64 -DWITH_MODULES=ON -DWITH_HOWDE=ON -DWITH_SOCKS=ON \
    && make -j8

RUN cd /opt/cpp-app-template && chmod +x ./bin/* #TODO

WORKDIR /tmp

ENTRYPOINT ["/opt/cpp-app-template/bin/cpp-app"] #TODO
