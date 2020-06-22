# AXSORT

> install boost
``` sudo apt-get install libboost-all-dev ```

> install google test (some command need sudo)
```
sudo apt install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```

> install google profiler
```
apt search google-perf   // find all packages we need
apt install libgoogle-perftools-dev google-perftools
```

- add compile link option to enable profiling (-lprofiler)
- use below command to generate pdf / txt file about profile result
- ``` google-pprof -pdf ./string_sorter sais.prof > sais.pdf ```
- ``` google-pprof -text ./string_sorter sais.prof > sais.txt ```

> run sample test
```
$ mkdir build
$ cd build
$ cp ../Makefile .
$ make simd_aligner
$ ./simd_aligner
```

## Note

- -std=c++2a need g++-10
    - [for ubuntu](https://askubuntu.com/questions/1192955/how-to-install-g-10-on-ubuntu-18-04)
- in string_sorter unit_test, there are some code blocks could be select.
- For now, string sorter sais need user padding '$' firstly.
    - it will be fix soon.
