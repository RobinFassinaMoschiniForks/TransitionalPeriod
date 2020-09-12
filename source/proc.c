
#include "common.h"

/*!
 *
 * @brief	Finds a process matching the specified
 * 		name by enumerating PID's. Probably a
 * 		better way of doing it, but I'll do this
 * 		for now.
 *
 * 		Note: It finds the _first_ process with
 * 		this name, and does not sort by unique
 * 		PID's.
 *
 *
 * @param	Filled in KM_API structure.
 * @param	DJB2 hash of the process name.
 *
!*/
KMFUNC PVOID ProcEnumProcess( IN KM_API * Api, IN ULONG Hsh )
{
	PVOID     prc;
	PCHAR     str;
	PVOID     img;
	ULONG     hsh;
	ULONG_PTR pid;

	img = NULL;

	for( pid=0;pid<ULONG_MAX;++pid ) {
		if ( ! Api->PsLookupProcessByProcessId( CPTR(pid), &prc ) ) {
			str = Api->PsGetProcessImageFileName( prc );
			hsh = HashString( str, 0 );

			if ( hsh == Hsh ) {
				img = prc;
				break;
			};

			Api->ObDereferenceObject( prc );
		};
		prc = NULL;
	};
	return img;
};

/*!
 *
 * @brief	Finds a thread stored within the context
 * 		of the target process.
 *
 * @param	Filled in KM_API structure.
 * @param	Pointer to a valid EPROCESS.
 *
!*/
KMFUNC PVOID ProcEnumThreads( IN KM_API * Api, IN PVOID Prc )
{
	PVOID     thd;
	PVOID     epc;
	PVOID     ctx;
	ULONG_PTR tid;

	ctx = NULL;

	for( tid=0;tid<ULONG_MAX;++tid ) {
		if ( ! Api->PsLookupThreadByThreadId( CPTR(tid), &thd ) ) {
			epc = Api->IoThreadToProcess( CPTR(thd) );

			if ( Prc == epc ) {
				ctx = thd;
				break;
			};

			Api->ObDereferenceObject( thd );
		};
	};
	return ctx;
};

/*!
 *
 * @brief	Allocates memory in the current process.
 *
 * @param	Filled in KM_API structure.
 * @param	Length of the memory to allocate.
 * @param	Memory Permissions to allocate.
 *
!*/
KMFUNC PVOID ProcAllocateMem( IN KM_API * Api, IN ULONG Len, IN ULONG Prm )
{
	NTSTATUS ret;
	LPVOID   mem;
	SIZE_T   len;

	len = Len;
	mem = NULL;

	ret = Api->ZwAllocateVirtualMemory( ZwCurrentProcess(),
			                    &mem,
					    0,
					    &len,
					    MEM_COMMIT | MEM_RESERVE,
					    Prm ); 
	if ( !ret )
	{
		return mem;
	};

	return NULL;
};

/*!
 *
 * @brief	Acquires the target thread's execution state
 * 		at current time.
 *
 * @param	Filled in KM_API structure.
 * @param	Pointer to the target thread.
 *
!*/
KMFUNC PVOID ProcGetThreadCtx( IN KM_API * Api, IN PVOID Thd )
{
	PCONTEXT ctx;
	SIZE_T   len;

	len = 0;

	if ( (ctx = ProcAllocateMem( Api, sizeof(CONTEXT), PAGE_READWRITE )) != NULL )
	{
		ctx->ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;

		if ( ! Api->PsGetContextThread( Thd, ctx, UserMode ) )
		{
			return ctx;
		};
		Api->ZwFreeVirtualMemory( ZwCurrentProcess(),
				          CPTR( &ctx ),
					  CPTR( &len ),
					  MEM_RELEASE );
	};
	return NULL;
};

/*!
 *
 * @brief	Sets the target thread's execution state 
 * 		to the specified context.
 *
 * @param	Filled in KM_API structure.
 * @param	Pointer to the taget thread.
 * @param	Pointer to the context structure.
 *
!*/
KMFUNC BOOL ProcSetThreadCtx( IN KM_API * Api, IN PVOID Thd, IN PCONTEXT Ctx )
{
	if ( ! Api->PsSetContextThread( Thd, Ctx, UserMode ) )
	{
		return TRUE;
	};
	return FALSE;
};
