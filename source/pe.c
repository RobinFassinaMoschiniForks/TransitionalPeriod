
#include "common.h"

SHARED PVOID PeGetFuncEat( IN PVOID Ptr, IN ULONG Hsh )
{
	PIMAGE_DOS_HEADER       dos;
	PIMAGE_NT_HEADERS       nth;
	PIMAGE_DATA_DIRECTORY   dir;
	PIMAGE_EXPORT_DIRECTORY exp;
	PDWORD                  aof;
	PDWORD                  aon;
	PUSHORT                 ano;
	PCHAR                   str;
	DWORD                   cnt;
	ULONG                   hxp;

	dos = CPTR( Ptr );
	nth = CPTR( UPTR(dos) + dos->e_lfanew );
	dir = CPTR( &nth->OptionalHeader.DataDirectory[0] );

	if ( dir->VirtualAddress ) {
		exp = CPTR( UPTR(dos) + dir->VirtualAddress );
		aof = CPTR( UPTR(dos) + exp->AddressOfFunctions );
		aon = CPTR( UPTR(dos) + exp->AddressOfNames );
		ano = CPTR( UPTR(dos) + exp->AddressOfNameOrdinals );

		for( cnt=0;cnt<exp->NumberOfNames;++cnt ) {	
			str = CPTR( UPTR(dos) + aon[cnt] );
			hxp = HashString(str, 0);

			if ( hxp == Hsh ) {
				return CPTR( UPTR(dos) + aof[ano[cnt]] );
			};
		};
	};
	return NULL;
};
