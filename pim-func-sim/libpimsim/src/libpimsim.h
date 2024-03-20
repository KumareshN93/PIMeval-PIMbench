// File: libpimsim.h
// PIM Functional Simulator Library Interface
// Copyright 2024 LavaLab @ University of Virginia. All rights reserved.

#ifndef LAVA_LIB_PIM_SIM_H
#define LAVA_LIB_PIM_SIM_H

#ifdef __cplusplus
extern "C" {
#endif

  //! @brief  PIM API return status
  enum PimStatus {
    PIM_ERROR = 0,
    PIM_OK,
  };

  //! @brief  PIM device types
  enum PimDeviceEnum {
    PIM_DEVICE_NONE = 0,
    PIM_FUNCTIONAL,
    PIM_DEVICE_BITSIMD_V,
  };

  //! @brief  PIM allocation types
  enum PimAllocEnum {
    PIM_ALLOC_V1 = 0,  // vertical layout, at most one region per core
    PIM_ALLOC_H1,      // horizontal layout, at most one region per core
  };

  //! @brief  PIM data copy types
  enum PimCopyEnum {
    PIM_COPY_V = 0,
    PIM_COPY_H,
  };

  typedef int PimCoreId;
  typedef int PimObjId;

  // Device creation and deletion
  PimStatus pimCreateDevice(PimDeviceEnum deviceType, unsigned numCores, unsigned numRows, unsigned numCols);
  PimStatus pimCreateDeviceFromConfig(PimDeviceEnum deviceType, const char* configFileName);
  PimStatus pimDeleteDevice();

  // Resource allocation and deletion
  PimObjId pimAlloc(PimAllocEnum allocType, unsigned numElements, unsigned bitsPerElement);
  PimObjId pimAllocAssociated(PimAllocEnum allocType, unsigned numElements, unsigned bitsPerElement, PimObjId ref);
  PimStatus pimFree(PimObjId obj);
  PimObjId pimRangedRef(PimObjId ref, unsigned idxBegin, unsigned idxEnd);

  // Data transfer
  PimStatus pimCopyHostToDevice(PimCopyEnum copyType, void* src, PimObjId dest);
  PimStatus pimCopyDeviceToHost(PimCopyEnum copyType, PimObjId src, void* dest);

  // High level computation
  PimStatus pimInt32Add(PimObjId src1, PimObjId src2, PimObjId dest);
  PimStatus pimInt32Abs(PimObjId src, PimObjId dest);
  PimStatus pimInt32Mul(PimObjId src1, PimObjId src2, PimObjId dest);
  PimStatus pimInt32Sub(PimObjId src1, PimObjId src2, PimObjId dest);
  PimStatus pimInt32Div(PimObjId src1, PimObjId src2, PimObjId dest);
  int pimInt32RedSum(PimObjId src);
  int pimInt32RedSumRanged(PimObjId src, unsigned idxBegin, unsigned idxEnd);
  PimStatus pimRotateR(PimObjId src);
  PimStatus pimRotateL(PimObjId src);

  // BitSIMD-V: Row-wide bit registers per subarray
  enum PimRowReg {
    PIM_RREG_SA = 0,
    PIM_RREG_R1,
    PIM_RREG_R2,
    PIM_RREG_R3,
    PIM_RREG_R4,
    PIM_RREG_R5,
  };

  // BitSIMD-V micro ops
  PimStatus pimOpReadRowToSa(PimObjId src, unsigned ofst);
  PimStatus pimOpWriteSaToRow(PimObjId src, unsigned ofst);
  PimStatus pimOpTRA(PimObjId src1, unsigned ofst1, PimObjId src2, unsigned ofst2, PimObjId src3, unsigned ofst3);
  PimStatus pimOpMove(PimObjId objId, PimRowReg src, PimRowReg dest);
  PimStatus pimOpSet(PimObjId objId, PimRowReg src, bool val);
  PimStatus pimOpNot(PimObjId objId, PimRowReg src, PimRowReg dest);
  PimStatus pimOpAnd(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpOr(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpNand(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpNor(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpXor(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpXnor(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpMaj(PimObjId objId, PimRowReg src1, PimRowReg src2, PimRowReg src3, PimRowReg dest);
  PimStatus pimOpSel(PimObjId objId, PimRowReg cond, PimRowReg src1, PimRowReg src2, PimRowReg dest);
  PimStatus pimOpRotateRH(PimObjId objId, PimRowReg src);
  PimStatus pimOpRotateLH(PimObjId objId, PimRowReg src);

#ifdef __cplusplus
}
#endif

#endif

