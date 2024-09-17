DGEMM, optimized on AMD Zen4 platforms
------
Build instructions

- Install aocc-compiler-4.2.0_1_amd64.deb from https://www.amd.com/en/developer/aocc.html
- Install aocl-blis-linux-aocc-4.2.0 (https://www.amd.com/en/developer/aocl/dense.html) to ../amd-blis
- Run `xmake`
------
Benchmarks

* Testing enviroment:

Vendor ID:                AuthenticAMD
  Model name:             AMD Ryzen 5 7600X 6-Core Processor
    CPU family:           25
    Model:                97
    Thread(s) per core:   2
    Core(s) per socket:   6
    Socket(s):            1
    Stepping:             2
    CPU(s) scaling MHz:   80%
    CPU max MHz:          5417.0000
    CPU min MHz:          400.0000
Caches (sum of all):
  L1d:                    192 KiB (6 instances)
  L1i:                    192 KiB (6 instances)
  L2:                     6 MiB (6 instances)
  L3:                     32 MiB (1 instance)


Naive version (baseline): 2.356 GFLOPS

AMD-blis CBLAS impl: 70.235 GFLOPS

Optimized version: 43.619 GFLOPS / 179.809 GFLOPS (6 thrds, OpenMP)
