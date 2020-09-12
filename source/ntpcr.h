
#ifndef NTPCR_H
#define NTPCR_H

#if defined(_WIN64)
	typedef union _KIDTENTRY64
	{
		union
		{
			struct
			{
				USHORT OffsetLow;
				USHORT Selector;

				struct
				{
					USHORT IstIndex		: 3;
					USHORT Reserved0	: 5;
					USHORT Type		: 5;
					USHORT Dpl		: 2;
					USHORT Present		: 1;
				};
				
				USHORT 	OffsetMiddle;
				ULONG 	OffsetHigh;
				ULONG	Reserved1;

			};

			ULONGLONG Alignment;
		};
	} KIDTENTRY64, *PKIDTENTRY64;

	typedef struct _KPCR
	{
		union
		{
			NT_TIB NtTib;

			struct
			{
				PVOID 		GdtBase;
				PVOID 		TssBase;
				PVOID 		UserRsp;
				struct _KPCR * 	Self;
				PVOID 		CurrentPcrb;
				PVOID 		LockArray;
				PVOID 		Used_Self;
			};
		};

		PKIDTENTRY64	IdtBase;
		ULONGLONG	Padding[2];
		UCHAR		Irql;
	} KPCR, *PKPCR;

	#define PcrGetPtr() ( PKPCR ) __readgsqword( FIELD_OFFSET(KPCR, Self) )
#else
	typedef struct _KIDTENTRY
	{
		USHORT OffsetLow;
		USHORT Selector;
		USHORT Access;
		USHORT ExtendedOffset;
	} KIDTENTRY, *PKIDTENTRY;

	typedef struct _KPCR
	{
		union
		{
			NT_TIB NtTib;

			struct
			{
				PVOID Used_ExceptionList;
				PVOID Used_StackBase;
				PVOID Spare2;
				PVOID TssCopy;
				ULONG ContextSwitches;
				ULONG SetMemberCopy;
				PVOID Used_Self;
			};
		};

		struct _KPCR * 	Self;
		PVOID		Prcb;
		UCHAR		Irql;
		ULONG		IRR;
		ULONG		IrrActive;
		ULONG		IDR;
		PVOID		KdVersionBlock;
		PKIDTENTRY	IdtBase;
	} KPCR, *PKPCR;

	#define PcrGetPtr() ( PKPCR ) __readfsdword( FIELD_OFFSET(KPCR, Self) )
#endif
#endif
