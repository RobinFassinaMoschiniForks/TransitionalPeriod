
#include "common.h"

KMFUNC PVOID PcrGetIdtPtr( IN PVOID Ptr )
{
#if defined(_WIN64)
	ULONG_PTR ohi;
	ULONG_PTR olo;
	ULONG_PTR omi;
	ULONG_PTR ent;

	ohi = (( PKIDTENTRY64 )Ptr)->OffsetHigh;
	omi = (( PKIDTENTRY64 )Ptr)->OffsetMiddle;
	olo = (( PKIDTENTRY64 )Ptr)->OffsetLow;

	ohi = ohi << 32;
	omi = omi << 16;
	ent = ohi + omi + olo;

	return CPTR( ent );
#else
	ULONG_PTR ohi;
	ULONG_PTR olo;
	ULONG_PTR ent;

	ohi = (( PKIDTENTRY )Ptr)->ExtendedOffset;
	olo = (( PKIDTENTRY )Ptr)->OffsetLow;

	ohi = ohi << 16;
	ent = ohi + olo;

	return CPTR( ent );
#endif
};

KMFUNC PVOID PcrGetNtBase( IN PVOID Ptr )
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_NT_HEADERS nts;
	ULONG_PTR         ref;

	ref = UPTR( Ptr );

	do {
		dos = CPTR( ref );
		nts = CPTR( UPTR(dos) + dos->e_lfanew );

		if ( dos->e_magic == IMAGE_DOS_SIGNATURE )
			break;

		ref = UPTR( UPTR(ref) - 0x1000 );

	} while ( TRUE );

	return CPTR( ref );
};

KMFUNC PVOID PcrNtPointer( VOID )
{
	PKPCR     pcr;
	PVOID     ntp;
	ULONG_PTR idt;

	pcr = CPTR( PcrGetPtr() );
	idt = UPTR( PcrGetIdtPtr( pcr->IdtBase ) );
	idt = UPTR( UPTR(idt) &~ 0xFFF );
	ntp = CPTR( PcrGetNtBase( CPTR(idt) ) );

	return ntp;
};

KMFUNC PVOID PcrGetModule( IN ULONG Hsh )
{
	PSYSTEM_MODULE_INFORMATION smi;
	PSYSTEM_MODULE_ENTRY       sme;
	NTSTATUS                   ret;
	PCHAR                      str;
	PVOID                      img;
	PVOID                      ntp;
	ULONG                      len;
	ULONG                      cnt;
	ULONG                      hsh;
	KM_API                     api;

	img = NULL;
	ntp = PcrNtPointer();

	api.ExFreePool               = PeGetFuncEat( ntp, H_EXFREEPOOL );
	api.ExAllocatePool           = PeGetFuncEat( ntp, H_EXALLOCATEPOOL );
	api.ZwQuerySystemInformation = PeGetFuncEat( ntp, H_ZWQUERYSYSTEMINFORMATION );

	ret = api.ZwQuerySystemInformation( SystemModuleInformation, NULL, 0, &len );
	
	if ( ret != STATUS_SUCCESS ) {
		smi = CPTR( api.ExAllocatePool( PagedPool, len ) );

		if ( smi ) {
			ret = api.ZwQuerySystemInformation( SystemModuleInformation, smi, len, NULL );
			
			if ( ret == STATUS_SUCCESS ) {
				sme = smi->Module;

				for ( cnt=0;cnt<smi->Count;++cnt ) {
					str = CPTR( sme[cnt].FullPathName + sme[cnt].OffsetToFileName );
					hsh = HashString( str, 0 );

					if ( hsh == Hsh ) {
						img = sme[cnt].ImageBase;
					};
				};
			};

			api.ExFreePool( smi );
		};
	};

	return img;
};
