#include <algorithm>
#include <execution>
#include "CountingIterator.h"

// define local constant values
namespace {
using index_t = unsigned short;
}  // namespace

__device__ void fast_sv_2(index_t* f, index_t* f_next, unsigned char adjc[],
                          index_t adjv[][8]) {
    int n = 1; // random choice for example purposes
    std::for_each_n(counting_iterator(0), n, 
      [=](unsigned int i){
        for (unsigned char k = 0; k < adjc[i]; ++k) {
          printf("Oops\n");
        }
    });
}

int main(){
  printf("Hello World\n");
  return 0;
}
