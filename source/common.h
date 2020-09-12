
#ifndef COMMON_H
#define COMMON_H

#pragma intrinsic(memset)
#pragma intrinsic(memcpy)

//! External Includes
#include <intrin.h>
#include <limits.h>
#include <windows.h>
#include <ntstatus.h>

//! Structures / Hashes
#include "hashes.h"
#include "ntpeb.h"
#include "ntpcr.h"
#include "nt.h"

//! Remove NOP's
#define NOPPAD			__attribute__((aligned(1)))

//! Custom Sections
#define KENTRY			NOPPAD __attribute__((section(".text$B")))
#define KMFUNC			NOPPAD __attribute__((section(".text$C")))
#define UENTRY			NOPPAD __attribute__((section(".text$E")))
#define UMFUNC			NOPPAD __attribute__((section(".text$F")))
#define SHARED			NOPPAD __attribute__((section(".text$F")))

//! Macros
#define ZwCurrentProcess()	((HANDLE)-1)
#define UPTR(x)			((ULONG_PTR)x)
#define CPTR(x)			((PVOID)x)
#define FUNC(x)			__typeof__(x) * x

//! KM Function Table
typedef struct
{
	FUNC(PsLookupProcessByProcessId);
	FUNC(PsGetProcessImageFileName);
	FUNC(ZwQuerySystemInformation);
	FUNC(PsLookupThreadByThreadId);
	FUNC(ZwAllocateVirtualMemory);
	FUNC(KeUnstackDetachProcess);
	FUNC(KeStackAttachProcess);
	FUNC(ZwFreeVirtualMemory);
	FUNC(ObDereferenceObject);
	FUNC(PsGetContextThread);
	FUNC(PsSetContextThread);
	FUNC(IoThreadToProcess);
	FUNC(PsSuspendProcess);
	FUNC(PsResumeProcess);
	FUNC(KeGetCurrentIrql);
	FUNC(ExQueueWorkItem);
	FUNC(ExAllocatePool);
	FUNC(ExFreePool);
} KM_API;

//! UM Function Table
typedef struct
{

} UM_API;

//! Internal Includes
#include "hash.h"
#include "proc.h"
#include "peb.h"
#include "pcr.h"
#include "pe.h"

#if defined(_WIN64)
extern VOID _EnterUmMode();
extern VOID _LeaveUmMode();
#define EnterUmMode _EnterUmMode
#define LeaveUmMode _LeaveUmMode
#else
extern VOID EnterUmMode();
extern VOID LeaveUmMode();
#endif

#endif // END COMMON_H