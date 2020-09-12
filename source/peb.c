
#include "common.h"

UMFUNC PVOID PebGetModule( IN ULONG Hsh )
{
	PPEB                  peb;
	PPEB_LDR_DATA         ldr;
	PLDR_DATA_TABLE_ENTRY dte;
	PLIST_ENTRY           ent;
	PLIST_ENTRY           hdr;
	ULONG                 mod;

	peb = NtCurrentTeb()->ProcessEnvironmentBlock;
	ldr = peb->Ldr;
	hdr = & ldr->InLoadOrderModuleList;
	ent = hdr->Flink;

	for ( ; hdr != ent ; ent = ent->Flink ) {
		dte = CPTR( ent );
		mod = HashString( dte->BaseDllName.Buffer, dte->BaseDllName.Length );

		if ( mod == Hsh ) {
			return CPTR( dte->DllBase );
		};
	};
	return NULL;
};
