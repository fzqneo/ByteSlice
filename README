
'ByteSlice' is a main-memory data layout designed for highly efficient scan and lookup in column-store databases. The basic idea is to chop column values into multiple bytes and store the bytes at different contiguous memory spaces.
The implementation heavily utilizes Single-Instruction-Multiple-Data (SIMD) instruction sets on modern CPUs to achieve bare-metal speed processing.
The scan algorithms are optimized to reduce number of instructions, memory footprint, branch mis-predictions and other performance-critical factors.

# Build

You need CMake to generate build scripts. Makefile is tested.

To generate debug build:

```
    mkdir debug
    cd debug
    cmake -DCMAKE_BUILD_TYPE=debug ..
    make -j4
```

To generate release build:

```
    mkdir release
    cd release
    cmake -DCMAKE_BUILD_TYPE=release ..
    make -j4
```

NOTE: The default build type is 'debug', which may not give optimal performance.



# Running Examples

Example programs are in 'example/' directory.

```
    cd debug
    ./example/example1 -s 10000000
```

To see a full list of options:

```
    ./example/example1 -h
```

NOTE: You can check the source code of the examples to see how to use the ByteSlice library.



# Multithreading

Multithreading is controlled by OpenMP environment variables: (assume you use GCC)

```
    OMP_NUM_THREADS=2 ./example/example1
```

NOTE: The default number of threads depends on the system, which is usually the number of cores.
You may also want to set the thread affinity via GOMP_CPU_AFFINITY (assume you use GCC).



# Running Tests

```
    make check
```

(Re-)build tests without running.

```
    make check-build
```


#  Documentation (work in progress)

You need doxygen to generate documentations in html and latex.

```
     doxygen
```


# File structure

example/
Example programs.

third-party/
Third-party libraries

src/
ByteSlice library source files.

tests/
Test cases written in GoogleTest.



# Citing this work

Ziqiang Feng, Eric Lo, Ben Kao, and Wenjian Xu. 
"Byteslice: Pushing the envelop of main memory data processing with a new storage layout." 
In Proceedings of the 2015 ACM SIGMOD International Conference on Management of Data, 
pp. 31-46. ACM, 2015.


# Tested platfor

This package has been tested on the following platform:

Linux 3.13.0-66-generic (64-bit)
Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz
g++ 4.9.3

