// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

//
// Provide common definitions between the Redhawk and the Redhawk PAL implementation. This header file is used
// (rather than PalRedhawk.h) since the PAL implementation is built in a different environment than Redhawk
// code. For instance both environments may provide a definition of various common macros such as NULL.
//
// This header contains only environment neutral definitions (i.e. using only base C++ types and compositions
// of those types) and can thus be included from either environment without issue.
//

#ifndef __PAL_REDHAWK_COMMON_INCLUDED
#define __PAL_REDHAWK_COMMON_INCLUDED

#include "rhassert.h"

// We define the notion of capabilities: optional functionality that the PAL may expose. Use
// PalHasCapability() with the constants below to determine what is supported at runtime.
enum PalCapability
{
    WriteWatchCapability                = 0x00000001,   // GetWriteWatch() and friends
    LowMemoryNotificationCapability     = 0x00000002,   // CreateMemoryResourceNotification() and friends
    GetCurrentProcessorNumberCapability = 0x00000004,   // GetCurrentProcessorNumber()
};

#define DECLSPEC_ALIGN(x)   __declspec(align(x))

#ifdef _AMD64_
#define AMD64_ALIGN_16 DECLSPEC_ALIGN(16)
#else // _AMD64_
#define AMD64_ALIGN_16
#endif // _AMD64_

struct AMD64_ALIGN_16 Fp128 {
    UInt64 Low;
    Int64 High;
};


struct PAL_LIMITED_CONTEXT
{
    // Includes special registers, callee saved registers and general purpose registers used to return values from functions (not floating point return registers)
#ifdef _TARGET_ARM_
    UIntNative  R0;
    UIntNative  R4;
    UIntNative  R5;
    UIntNative  R6;
    UIntNative  R7;
    UIntNative  R8;
    UIntNative  R9;
    UIntNative  R10;
    UIntNative  R11;

    UIntNative  IP;
    UIntNative  SP;
    UIntNative  LR;

    UInt64      D[16-8]; // D8 .. D15 registers (D16 .. D31 are volatile according to the ABI spec)

    UIntNative GetIp() const { return IP; }
    UIntNative GetSp() const { return SP; }
    UIntNative GetFp() const { return R7; }
    UIntNative GetLr() const { return LR; }
    void SetIp(UIntNative ip) { IP = ip; }
    void SetSp(UIntNative sp) { SP = sp; }
#elif defined(_TARGET_ARM64_)
#if !defined(GEN_REG_COUNT)
#define GEN_REG_COUNT 29
#endif

    // ARM64TODO: we don't need to save X9-X15
    // ARM64TODO: do we need X16 (IP0), X17 (IP1), X18 or can we skip them? 
    UIntNative  X[GEN_REG_COUNT];

    UIntNative  FP;
    UIntNative  LR;
    UIntNative  SP;
    UIntNative  IP;

    UIntNative   D[16 - 8]; // Only the bottom 64-bit value of the V registers V8..V15 needs to be preserved
                            // (V0-V7 and V16-V31 are not preserved according to the ABI spec).


    UIntNative GetIp() const { return IP; }
    UIntNative GetSp() const { return SP; }
    UIntNative GetFp() const { return FP; }
    UIntNative GetLr() const { return LR; }
#elif defined(UNIX_AMD64_ABI)
    // Param regs: rdi, rsi, rdx, rcx, r8, r9, scratch: rax, rdx (both return val), preserved: rbp, rbx, r12-r15
    UIntNative  IP;
    UIntNative  Rsp;
    UIntNative  Rbp;
    UIntNative  Rax;
    UIntNative  Rbx;
    UIntNative  Rdx;
    UIntNative  R12;
    UIntNative  R13;
    UIntNative  R14;
    UIntNative  R15;

    UIntNative GetIp() const { return IP; }
    UIntNative GetSp() const { return Rsp; }
    void SetIp(UIntNative ip) { IP = ip; }
    void SetSp(UIntNative sp) { Rsp = sp; }
    UIntNative GetFp() const { return Rbp; }
#else // _TARGET_ARM_
    UIntNative  IP;
    UIntNative  Rsp;
    UIntNative  Rbp;
    UIntNative  Rdi;
    UIntNative  Rsi;
    UIntNative  Rax;
    UIntNative  Rbx;
#ifdef _TARGET_AMD64_
    UIntNative  R12;
    UIntNative  R13;
    UIntNative  R14;
    UIntNative  R15;
    UIntNative  __explicit_padding__;
    Fp128       Xmm6;
    Fp128       Xmm7;
    Fp128       Xmm8;
    Fp128       Xmm9;
    Fp128       Xmm10;
    Fp128       Xmm11;
    Fp128       Xmm12;
    Fp128       Xmm13;
    Fp128       Xmm14;
    Fp128       Xmm15;
#endif // _TARGET_AMD64_

    UIntNative GetIp() const { return IP; }
    UIntNative GetSp() const { return Rsp; }
    UIntNative GetFp() const { return Rbp; }
#endif // _TARGET_ARM_
};

void RuntimeThreadShutdown(void* thread);

#ifdef PLATFORM_UNIX
typedef void (__fastcall * ThreadExitCallback)();

extern ThreadExitCallback g_threadExitCallback;

typedef Int32 (*PHARDWARE_EXCEPTION_HANDLER)(UIntNative faultCode, UIntNative faultAddress, PAL_LIMITED_CONTEXT* palContext, UIntNative* arg0Reg, UIntNative* arg1Reg);
#endif

#endif // __PAL_REDHAWK_COMMON_INCLUDED
