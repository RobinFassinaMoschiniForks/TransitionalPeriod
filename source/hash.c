
#include "common.h"

SHARED ULONG HashString( IN PVOID Inp, IN ULONG Len )
{
	ULONG  hsh;
	PUCHAR ptr;
	UCHAR  cur;

	hsh = 5381;
	ptr = Inp;

	while ( TRUE )
	{
		cur = * ptr;

		if ( ! Len ) {
			if ( ! * ptr ) {
				break;
			};
		} else {
			if ( ( ULONG )( ptr - ( PUCHAR )Inp ) >= Len ) {
				break;
			};
			if ( ! * ptr ) {
				++ptr; continue;
			};
		};

		if ( cur >= 'a' )
			cur -= 0x20;

		hsh = ((hsh << 5) + hsh) + cur; ++ptr;
	};

	return hsh;
};
