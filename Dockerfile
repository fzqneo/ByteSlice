FROM ubuntu:trusty
MAINTAINER Ziqiang Feng <zf@cs.cmu.edu>

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++

ADD . /root/ByteSlice
WORKDIR /root/ByteSlice
RUN mkdir release
WORKDIR release
RUN cmake -DCMAKE_BUILD_TYPE=release .. && make

CMD /root/ByteSlice/release/example/example1 -s 10000000

