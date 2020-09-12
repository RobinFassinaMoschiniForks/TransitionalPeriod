
#ifndef NT_H
#define NT_H

typedef enum _KPROCESSOR_MODE
{
	KernelMode,
	UserMode
} KPROCESSOR_MODE;

typedef enum _KIRQL
{
	PASSIVE_LEVEL	= 0,
	APC_LEVEL	= 1,
	DISPATCH_LEVEL	= 2
} KIRQL;

typedef enum _WORK_QUEUE_TYPE
{
	CriticalWorkQueue,
	DelayedWorkQueue
} WORK_QUEUE_TYPE;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemModuleInformation = 11
} SYSTEM_INFORMATION_CLASS;

typedef enum _POOL_TYPE
{
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	ReservedType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned
} POOL_TYPE;

typedef struct _KAPC_STATE
{
	LIST_ENTRY	ApcListHead[2];
	PVOID		Process;
	UCHAR		KernelApcInProgress;
	UCHAR		KernelApcPending;
	UCHAR		UserApcPending;
} KAPC_STATE, *PKAPC_STATE;

typedef struct _WORK_QUEUE_ITEM
{
	LIST_ENTRY	List;
	PVOID		Routine;
	PVOID		Parameter;
} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM;

typedef struct _SYSTEM_MODULE_ENTRY
{
	HANDLE	Section;
	PVOID	MappedBase;
	PVOID	ImageBase;
	ULONG	ImageSize;
	ULONG	Flags;
	USHORT	LoadOrderIndex;
	USHORT	InitOrderIndex;
	USHORT	LoadCount;
	USHORT	OffsetToFileName;
	UCHAR	FullPathName[ MAX_PATH - 4 ];
} SYSTEM_MODULE_ENTRY, *PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG			Count;
	SYSTEM_MODULE_ENTRY 	Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

NTSTATUS
NTAPI
ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	IN PVOID ReturnLength
	);

PVOID
NTAPI
ExAllocatePool(
	IN POOL_TYPE PoolType,
	IN SIZE_T NumberOfBytes
	);

VOID
NTAPI
ExFreePool(
	IN PVOID a
	);

VOID
NTAPI
ExQueueWorkItem(
	IN PWORK_QUEUE_ITEM WorkItem,
	IN WORK_QUEUE_TYPE QueueType
	);

KIRQL
NTAPI
KeGetCurrentIrql(
	IN VOID
	);

NTSTATUS
NTAPI
PsLookupProcessByProcessId(
	IN HANDLE ProcessId,
	IN PVOID* Process
	);

NTSTATUS
NTAPI
PsLookupThreadByThreadId(
	IN HANDLE ThreadId,
	IN PVOID* Thread
	);

PCHAR
NTAPI
PsGetProcessImageFileName(
	IN PVOID Process
	);

VOID
NTAPI
ObDereferenceObject(
	IN PVOID Object
	);

NTSTATUS
NTAPI
PsResumeProcess(
	IN PVOID Process
	);

NTSTATUS
NTAPI
PsSuspendProcess(
	IN PVOID Process
	);

PVOID
NTAPI
IoThreadToProcess(
	IN PVOID Thread
	);

VOID
NTAPI
KeStackAttachProcess(
	IN PVOID Process,
	IN PKAPC_STATE State
	);

VOID
NTAPI
KeUnstackDetachProcess(
	IN PKAPC_STATE State
	);

NTSTATUS
NTAPI
ZwAllocateVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID* BaseAddress,
	IN ULONG_PTR ZeroBits,
	IN PSIZE_T RegionSize,
	IN ULONG AllocationType,
	IN ULONG Protect
	);

NTSTATUS
NTAPI
ZwFreeVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID* BaseAddress,
	IN PSIZE_T RegionSize,
	IN ULONG FreeType
	);

NTSTATUS
NTAPI
PsGetContextThread(
	IN PVOID Thread,
	IN PCONTEXT ThreadContext,
	IN KPROCESSOR_MODE Mode
	);

NTSTATUS
NTAPI
PsSetContextThread(
	IN PVOID Thread,
	IN PCONTEXT ThreadContext,
	IN KPROCESSOR_MODE Mode
	);

#endif
