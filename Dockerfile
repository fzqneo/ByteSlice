FROM ubuntu:trusty
MAINTAINER Ziqiang Feng <zf@cs.cmu.edu>

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++ \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

ADD . /root/ByteSlice
WORKDIR /root/ByteSlice
RUN mkdir release
RUN cd release && cmake -DCMAKE_BUILD_TYPE=release .. && make -j4

CMD /root/ByteSlice/release/example/example1 -s 16000000

