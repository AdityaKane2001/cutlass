/***************************************************************************************************
 * Copyright (c) 2017-2021, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *   * Neither the name of the NVIDIA CORPORATION nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************************************/

/*! \file
  \brief 
    Defines a GEMM with Reduction based on an existing UniversalGemm kernel.

*/

#pragma once

#include "cutlass/cutlass.h"

#include "cutlass/conv/kernel/default_conv2d_fprop.h"
#include "cutlass/conv/kernel/implicit_gemm_convolution_with_fused_epilogue.h"

#include "cutlass/epilogue/threadblock/default_epilogue_with_broadcast.h"
#include "cutlass/epilogue/threadblock/epilogue_with_broadcast.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

namespace cutlass {
namespace conv {
namespace kernel {

/////////////////////////////////////////////////////////////////////////////////////////////////

template <
  typename ElementA,
  typename LayoutA,
  typename ElementB,
  typename LayoutB,
  typename ElementC,
  typename LayoutC,
  typename ElementAccumulator,
  typename OperatorClass,
  typename ArchTag,
  typename ThreadblockShape,
  typename WarpShape,
  typename InstructionShape,
  typename EpilogueOutputOp,
  typename ThreadblockSwizzle,
  int Stages,
  typename MathOperatorTag,
  conv::IteratorAlgorithm IteratorAlgorithm = IteratorAlgorithm::kAnalytic,
  conv::StrideSupport StrideSupport = StrideSupport::kStrided
>
struct DefaultConv2dFpropWithBroadcast {

  using ImplicitGemmBase = typename DefaultConv2dFprop<
    ElementA, LayoutA,
    ElementB, LayoutB,
    ElementC, LayoutC,
    ElementAccumulator,
    OperatorClass,
    ArchTag,
    ThreadblockShape,
    WarpShape,
    InstructionShape,
    EpilogueOutputOp,
    ThreadblockSwizzle,
    Stages,
    MathOperatorTag,
    IteratorAlgorithm,
    StrideSupport
  >::Kernel;

  // Replace epilogue
  using Epilogue = typename cutlass::epilogue::threadblock::DefaultEpilogueWithBroadcastTensorOp<
    typename ImplicitGemmBase::Epilogue::Shape,
    typename ImplicitGemmBase::Epilogue::WarpMmaOperator,
    ImplicitGemmBase::Epilogue::kPartitionsK,
    ElementC,
    typename EpilogueOutputOp::ElementT,
    ElementC,
    EpilogueOutputOp,
    ImplicitGemmBase::Epilogue::kElementsPerAccess
  >::Epilogue;

  // Define the kernel
  using Kernel = cutlass::conv::kernel::ImplicitGemmConvolutionWithFusedEpilogue<
    typename ImplicitGemmBase::Mma,
    Epilogue,
    ThreadblockSwizzle,
    conv::Operator::kFprop
  >;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace kernel
}  // namespace conv
}  // namespace cutlass

/////////////////////////////////////////////////////////////////////////////////////////////////
