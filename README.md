# nvc++: Compiler failed to translate accelerator region when using `__device__`

Clone the repo:

```
git clone git@github.com:ugGit/nvhpc225-o3-issue-recreation.git
```

## Requirements
The issue has been encountered in an environment using the following modules, which must be available during the compilation:

* gcc/11.2.0
* nvhpc/22.5 (using CUDA 11.7)
* nvhpc/22.3 (using CUDA 11.6)

## Information about the setup
The code has been tested on an Nvidia A6000 and a GeForce RTX 2080.

This project contains the minimal required modules and code to recreate the issue encountered.

## Relevant code
```cpp
__device__ void fast_sv_2(index_t* f, index_t* f_next, 
                          unsigned char adjc[], index_t adjv[][8]) {
    int n = 1; // random choice for example purposes
    std::for_each_n(counting_iterator(0), n, 
      [=](unsigned int i){
        for (unsigned char k = 0; k < adjc[i]; ++k) {
          printf("Oops\n");
        }
    });
}
```

## Issue description
The compilation of a C++ standard `for_each_n` loop fails.
This is caused by a particular combination of factors, including the usage of higher optimization policies. 
First, the function needs to be specified as one that runs on the `__device__`.
Second, the number of iterations (`adjc[i]`) for the inner loop must be unknown at compile time.

## Execute the code

Use nvhpc/22.5 for this compilation.

```
nvc++ -stdpar=gpu -O3 -o main main.cpp && ./main
```

Expected output:
```
nvvmCompileProgram error 9: NVVM_ERROR_COMPILATION.
Error: /tmp/pgacc3p5ALDgkVyJ3.gpu (354, 27): parse '%li119' defined with type 'i8'
NVC++-F-0155-Compiler failed to translate accelerator region (see -Minfo messages): Device compiler exited with error status code (main.cpp: 1)
NVC++/x86-64 Linux 22.5-0: compilation aborted
```

## Observations

The compilation succeeds if:
* no optimization or levels equal or lower than `-O1` are used.
* nvhpc/22.3 is used (also for `-O2` and `-O3`).
* the `__device__` keyword is removed, or replaced by `__host__`.
* the number of iterations is replaced with a value known at compile time (e.g. replacing the expression `adjc[i]` with `3`).
* compiling for multicore `-stdpar=multicore`.

When using `-O2`, the compilation still fails, but with a slightly different error message:
```
nvvmCompileProgram error 9: NVVM_ERROR_COMPILATION.
Error: /tmp/pgaccJZKFP5tq7Sll.gpu (375, 30): parse '%li116_ldcv' defined with type 'i32'
NVC++-F-0155-Compiler failed to translate accelerator region (see -Minfo messages): Device compiler exited with error status code (main.cpp: 1)
NVC++/x86-64 Linux 22.5-0: compilation aborted
```

When compiling with `-nostdpar` the compilation error is more clear:
```
"main.cpp", line 10: error: this declaration has no storage class or type specifier
  __device__ void fast_sv_2(index_t* f, index_t* f_next, unsigned char adjc[],
  ^
```

Adding the flag `-Minfo messages` during compilation yields the following output:
```
main.cpp:
std::chrono::duration<long, std::ratio<(long)1, (long)1000000000>>::_S_gcd(long, long):
      2, include "execution"
         131, include "algorithm_execution.hpp"
               21, include "stdpar_config.hpp"
                    34, include "caching_allocator.h"
                         20, include "allocator.h"
                              30, include "validator.h"
                                   22, include "memory_resource.h"
                                        27, include "memory_resource"
                                             38, include "shared_mutex"
                                                  36, include "chrono"
                                                      460, Loop not vectorized/parallelized: not countable
std::chrono::duration<long, std::ratio<(long)1, (long)1>>::_S_gcd(long, long):
      2, include "execution"
         131, include "algorithm_execution.hpp"
               21, include "stdpar_config.hpp"
                    34, include "caching_allocator.h"
                         20, include "allocator.h"
                              30, include "validator.h"
                                   22, include "memory_resource.h"
                                        27, include "memory_resource"
                                             38, include "shared_mutex"
                                                  36, include "chrono"
                                                      460, Loop not vectorized/parallelized: not countable
T1 std::for_each_n<counting_iterator, int, fast_sv_2(unsigned short *, unsigned short *, unsigned char *, unsigned short (*)[8])::[lambda(unsigned int) (instance 1)]>(T1, T2, T3):
        3820, Loop not vectorized/parallelized: contains call
T2 std::for_each<counting_iterator, fast_sv_2(unsigned short *, unsigned short *, unsigned char *, unsigned short (*)[8])::[lambda(unsigned int) (instance 1)]>(T1, T1, T2):
          15, Loop not vectorized/parallelized: contains call
```          

## Conclusions
Obviously, the MRE depicts a somewhat weird use case declaring a function for execution on the device, while compiling it with nvc++.
However, this primarily coding error was introduced by a moment of inattention when rewriting a previous CUDA algorithm.
Since the very same code compiled and ran successfully with nvhpc/22.3, the error with 22.5 comes surprisingly. 
Especially, if considering that it only happens when using a high level of optimizations.
