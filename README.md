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

> run sample test
```
$ mkdir build
$ cd build
$ cp ../Makefile .
$ make sample
$ ./sample
```
