// Test: C++ version of vector addition
// Copyright 2024 LavaLab @ University of Virginia. All rights reserved.

#include <iostream>
#include <vector>
#include <getopt.h>
#include <stdint.h>
#include <iomanip>
#if defined(_OPENMP)
#include <omp.h>
#endif

#include "../util.h"
#include "libpimeval.h"
#include <cassert>

using namespace std;

// Params ---------------------------------------------------------------------
typedef struct Params
{
  uint64_t vectorLength;
  char *configFile;
  char *inputFile;
  bool shouldVerify;
} Params;

void usage()
{
  fprintf(stderr,
          "\nUsage:  ./axpy [options]"
          "\n"
          "\n    -l    input size (default=65536 elements)"
          "\n    -c    dramsim config file"
          "\n    -i    input file containing two vectors (default=generates vector with random numbers)"
          "\n    -v    t = verifies PIM output with host output. (default=false)"
          "\n");
}

struct Params getInputParams(int argc, char **argv)
{
  struct Params p;
  p.vectorLength = 65536;
  p.configFile = nullptr;
  p.inputFile = nullptr;
  p.shouldVerify = false;

  int opt;
  while ((opt = getopt(argc, argv, "h:l:c:i:v:")) >= 0)
  {
    switch (opt)
    {
    case 'h':
      usage();
      exit(0);
      break;
    case 'l':
      p.vectorLength = strtoull(optarg, NULL, 0);
      break;
    case 'c':
      p.configFile = optarg;
      break;
    case 'i':
      p.inputFile = optarg;
      break;
    case 'v':
      p.shouldVerify = (*optarg == 't') ? true : false;
      break;
    default:
      fprintf(stderr, "\nUnrecognized option!\n");
      usage();
      exit(0);
    }
  }
  return p;
}

void axpy(uint64_t vectorLength, const std::vector<int> &X, const std::vector<int> &Y, int A, std::vector<int> &dst)
{
  unsigned bitsPerElement = sizeof(int) * 8;
  PimObjId obj1 = pimAlloc(PIM_ALLOC_AUTO, vectorLength, bitsPerElement, PIM_INT32);
  assert(obj1 != -1);

  PimObjId obj2 = pimAllocAssociated(bitsPerElement, obj1, PIM_INT32);
  assert(obj2 != -1);

  PimStatus status = pimCopyHostToDevice((void *)X.data(), obj1);
  assert (status == PIM_OK);

  status = pimMulScalar(obj1, obj1, A);
  assert (status == PIM_OK);

  status = pimCopyHostToDevice((void *)Y.data(), obj2);
  assert (status == PIM_OK);

  status = pimAdd(obj1, obj2, obj2);
  assert (status == PIM_OK);

  dst.resize(vectorLength);
  status = pimCopyDeviceToHost(obj2, (void *)dst.data());
  assert (status == PIM_OK);

  pimFree(obj1);
  pimFree(obj2);
}

int main(int argc, char* argv[])
{
  struct Params params = getInputParams(argc, argv);
  std::cout << "Vector length: " << params.vectorLength << "\n";
  std::vector<int> X, Y, Y_device;
  if (params.inputFile == nullptr)
  {
    getVector(params.vectorLength, X);
    getVector(params.vectorLength, Y);
  } else {
    //TODO: Read from files
  }
  
  if (!createDevice(params.configFile)) return 1;

  //TODO: Check if vector can fit in one iteration. Otherwise need to run in multiple iteration.
  int A = rand() % 50;
  axpy(params.vectorLength, X, Y, A, Y_device);

  if (params.shouldVerify) {
    // verify result
    #pragma omp parallel for
    for (unsigned i = 0; i < params.vectorLength; ++i)
    {
      int result = A * X[i] + Y[i];
      if (Y_device[i] != result)
      {
        std::cout << "Wrong answer: " << Y_device[i] << " (expected " << result << ")" << std::endl;
      }
    }
  }

  pimShowStats();

  return 0;
}