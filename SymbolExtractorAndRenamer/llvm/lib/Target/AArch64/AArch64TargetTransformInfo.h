//===-- AArch64TargetTransformInfo.h - AArch64 specific TTI -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file a TargetTransformInfo::Concept conforming object specific to the
/// AArch64 target machine. It uses the target's detailed information to
/// provide more precise answers to certain TTI queries, while letting the
/// target independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AARCH64_AARCH64TARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_AARCH64_AARCH64TARGETTRANSFORMINFO_H

#include "AArch64.h"
#include "AArch64TargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Target/TargetLowering.h"
#include <algorithm>

namespace llvm {

class AArch64TTIImpl : public BasicTTIImplBase<AArch64TTIImpl> {
  typedef BasicTTIImplBase<AArch64TTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const AArch64Subtarget *ST;
  const AArch64TargetLowering *TLI;

  /// Estimate the overhead of scalarizing an instruction. Insert and Extract
  /// are set if the result needs to be inserted and/or extracted from vectors.
  unsigned getScalarizationOverhead(Type *Ty, bool Insert, bool Extract);

  const AArch64Subtarget *getST() const { return ST; }
  const AArch64TargetLowering *getTLI() const { return TLI; }

  enum MemIntrinsicType {
    VECTOR_LDST_TWO_ELEMENTS,
    VECTOR_LDST_THREE_ELEMENTS,
    VECTOR_LDST_FOUR_ELEMENTS
  };

public:
  explicit AArch64TTIImpl(const AArch64TargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  /// \name Scalar TTI Implementations
  /// @{

  using BaseT::getIntImmCost;
  int getIntImmCost(int64_t Val);
  int getIntImmCost(const APInt &Imm, Type *Ty);
  int getIntImmCost(unsigned Opcode, unsigned Idx, const APInt &Imm, Type *Ty);
  int getIntImmCost(Intrinsic::ID IID, unsigned Idx, const APInt &Imm,
                    Type *Ty);
  TTI::PopcntSupportKind getPopcntSupport(unsigned TyWidth);

  /// @}

  /// \name Vector TTI Implementations
  /// @{

  bool enableInterleavedAccessVectorization() { return true; }

  unsigned getNumberOfRegisters(bool Vector) {
    if (Vector) {
      if (ST->hasNEON())
        return 32;
      return 0;
    }
    return 31;
  }

  unsigned getRegisterBitWidth(bool Vector) {
    if (Vector) {
      if (ST->hasNEON())
        return 128;
      return 0;
    }
    return 64;
  }

  unsigned getMinVectorRegisterBitWidth() {
    // FIXME: This should probably be enabled for any Neon subtarget but
    // currently it was only tuned for Cyclone.
    return ST->getMinVectorRegisterBitWidth();
  }

  unsigned getMaxInterleaveFactor(unsigned VF);

  int getCastInstrCost(unsigned Opcode, Type *Dst, Type *Src);

  int getExtractWithExtendCost(unsigned Opcode, Type *Dst, VectorType *VecTy,
                               unsigned Index);

  int getVectorInstrCost(unsigned Opcode, Type *Val, unsigned Index);

  int getArithmeticInstrCost(
      unsigned Opcode, Type *Ty,
      TTI::OperandValueKind Opd1Info = TTI::OK_AnyValue,
      TTI::OperandValueKind Opd2Info = TTI::OK_AnyValue,
      TTI::OperandValueProperties Opd1PropInfo = TTI::OP_None,
      TTI::OperandValueProperties Opd2PropInfo = TTI::OP_None,
      ArrayRef<const Value *> Args = ArrayRef<const Value *>());

  int getAddressComputationCost(Type *Ty, ScalarEvolution *SE, const SCEV *Ptr);

  int getCmpSelInstrCost(unsigned Opcode, Type *ValTy, Type *CondTy);

  int getMemoryOpCost(unsigned Opcode, Type *Src, unsigned Alignment,
                      unsigned AddressSpace);

  int getCostOfKeepingLiveOverCall(ArrayRef<Type *> Tys);

  void getUnrollingPreferences(Loop *L, TTI::UnrollingPreferences &UP);

  Value *getOrCreateResultFromMemIntrinsic(IntrinsicInst *Inst,
                                           Type *ExpectedType);

  bool getTgtMemIntrinsic(IntrinsicInst *Inst, MemIntrinsicInfo &Info);

  int getInterleavedMemoryOpCost(unsigned Opcode, Type *VecTy, unsigned Factor,
                                 ArrayRef<unsigned> Indices, unsigned Alignment,
                                 unsigned AddressSpace);

  unsigned getCacheLineSize();

  unsigned getPrefetchDistance();

  unsigned getMinPrefetchStride();

  unsigned getMaxPrefetchIterationsAhead();
  /// @}
};

} // end namespace llvm

#endif
