
#ifndef PROC_H
#define PROC_H

/*!
 *
 * @brief       Finds a process matching the specified
 *              name by enumerating PID's. Probably a
 *              better way of doing it, but I'll do this
 *              for now.
 *
 *              Note: It finds the _first_ process with
 *              this name, and does not sort by unique
 *              PID's.
 *
 *
 * @param       Filled in KM_API structure.
 * @param       DJB2 hash of the process name.
 *
!*/
PVOID ProcEnumProcess( IN KM_API * Api, IN ULONG Hsh );

/*!
 *
 * @brief       Finds a thread stored within the context
 *              of the target process.
 *
 * @param       Filled in KM_API structure.
 * @param       Pointer to a valid EPROCESS.
 *
!*/
PVOID ProcEnumThreads( IN KM_API * Api, IN PVOID Prc );

/*!
 *
 * @brief       Allocates Read - Write - Exec Memory In The
 *              target process.
 *
 * @param       Filled in KM_API structure.
 * @param       Length of the memory to allocate.
 * @param	Memory Permissions to allocate.
 *
!*/
PVOID ProcAllocateMem( IN KM_API * Api, IN ULONG Len, IN ULONG Prm );

/*!
 *
 * @brief       Acquires the target thread's execution state
 *              at current time.
 *
 * @param       Filled in KM_API structure.
 * @param       Pointer to the target thread.
 *
!*/
PVOID ProcGetThreadCtx( IN KM_API * Api, IN PVOID Thd );

/*!
 *
 * @brief       Sets the target thread's execution state
 *              to the specified context.
 *
 * @param       Filled in KM_API structure.
 * @param       Pointer to the taget thread.
 * @param       Pointer to the context structure.
 *
!*/
BOOL ProcSetThreadCtx( IN KM_API * Api, IN PVOID Thd, IN PCONTEXT Ctx );

#endif
