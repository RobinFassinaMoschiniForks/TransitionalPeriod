# TransitionalPeriod: Multi Ring Kernel To UserMode Payload

As a hobbyist programmer and average pentester, I tend to constantly look into improving my own tactics, and inherently study & replicate advanced capabilites of various malicious actors, such as the Equation Group or "Fancy Bear". One of the few things that pushed me into a field of interest of developing position independent kernel malware was the introduction of "DoublePulsar" from the original "[Lost In Transalation](https://github.com/misterch0c/shadowbroker)", which whilst advanced, was relatively simple in design: It hooked function virtual tables in a read / write section of srv.sys to redirect execution to its malicious function. Additionally, its kernel to usermode transitional payload, studied by Countercept ( now F-Secure ) under "[Analyzing The DOUBLEPULSAR Kernel DLL Injection Technique](https://blog.f-secure.com/analyzing-the-doublepulsar-kernel-dll-injection-technique/)" leveraged Kernel Asynchronous Procedural Call's to achieve execution in the context of the target process, and before then attempting to load the appended DLL within the memory of the host process.

However: A question came to mind, has anyone truly replicated this tactic? Further research seems to show this is not the case, or at least not in the form of a true "shellcode" (Position Independent Code). Furthermore, is there ways we as actors could improve it, to avoid detections of Microsoft's Kernel to User Injection via APC's?

![](https://i.imgur.com/T0dOWNZ.gif)

Yes, of course. Everything has a signature, and every signature is breakable when you change it. However, nothing is FUD. Its a nice little game of cat & mouse. However, this is no jab at defenders: I've got far more respect for the BlueTeam chasing the Red Team, than the Red Team cobbling together public tactics to create an atrocities.

## Toolsets

In this project, I'll be using:

* [Netwide Assembler (nasm)](https://www.nasm.us/): One of my favorite assemblers to date. Its available in pretty much every known package manager.
* [Mingw-W64](http://mingw-w64.org/doku.php): Ah, the infamous "GCC for Windows". One of my favorite compilers to date. I'm not a big fan of MSVC, and have not tested TDM-GCC
* [Python](https://www.python.org/download/releases/3.0/): Because why not! I'm too lazy sometimes to write something in C when it can be done for build purposes.
* [python-pefile](https://github.com/erocarrera/pefile): One of my favorite libraries to date for Python concerning PEFILE parsing. Its also one of the most popular, and used even in VirusTotal.

## Desicions:

Writing position independent code in a language such as C is nothing new: Its been done by various threat actors, and blogged about by individuals such as [Matt Graeber](http://www.exploit-monday.com/2013/08/writing-optimized-windows-shellcode-in-c.html), and [Nick Harbor](https://nickharbour.wordpress.com/2010/07/01/writing-shellcode-with-a-c-compiler/). Despite their use case being MSVC specific, achieving the same with mingw-w64 is'nt much of a trouble, and personally much easier.

Firstly, we'll be needing to control the order the code is placed in. This is to ensure that our entrypoint, in the case of AMD64, aligns the stack before passing execution to our C entrypoint. This is required of the Windows ABI ( Application Binary Interface ) as the stack must always be 16-byte aligned, as stated by [MSDN's offical docs](https://docs.microsoft.com/en-us/cpp/build/x64-software-conventions?view=vs-2017).

Furthermore, we'll want to control where our usermode code is placed. This is because we'll be copying our usermode portion of the code into a **virtual** address space of a process. Once within that portion of memory, it will be unable to access the pieces of code stored within the physical address space.

Luckily, according to the "[Microsoft Portable Executable and Common Object File Format Specification](http://www.skyfree.org/linux/references/coff.pdf#:~:text=This%20document%20specifies%20the%20structure,format%20is%20not%20architecture%2Dspecific.)" by Microsoft themselves under "4.2 Grouped Sections", one can control the order of your code by _alphabetical order_ with a `$` (dollar sign) symbol in front of the section name. So by specifying a section name of `.text$A` and `.text$B`, all code compiled in section A, would end up before code in section B in the `.text` section.

![](https://i.imgur.com/Yj7L4ru.png)

By designing it this way we will be able to:
* Locate the start and end of the usermode portion of the shellcode by comparing the distance between the `LeaveUmMode` and `EnterUmMode` functions.
* Safely allow the usermode code to still use the string hashing and pe parsing functions without having to modify the instruction pointer (RIP/EIP) relative jumps and calls within its virtual address space of the target process.
* Safely copy the usermode portion from kernel, as well as being able to set the instruction pointer to the start of the entrypoint in the target process - which is ordered before all the other pieces of the usermode / shared code.

I won't dig into locating modules in kernel mode, as it has been documented extensively by individuals such as [Zerosum0x0](https://zerosum0x0.blogspot.com/2017/04/doublepulsar-initial-smb-backdoor-ring.html). I'm using the same tactic of finding the kernel base by reading the first Interupt Descriptor Table entry, and aligning it to the base of the allocated page. Once this has completed - I am then subtracting by PAGE_SIZE increments until I reach the base of the page which contains the `IMAGE_DOS_SIGNATURE` "MZ" stub.

I then parse the PE's IMAGE_EXPORT_TABLE to find the ZwQuerySystemInformation, ExAllocatePoolWithTag, and ExFreePool functions so that I can allocate a buffer for all loaded modules, and parse it manually to find the base address of a specified module. The code for all of this is under [line 21 to line 137 of pcr.c](https://github.com/SecIdiot/TransitionalPeriod/blob/master/source/pcr.c#L21-L137)

## Dealing with Interrupt Request Level

One of the potential issues in kernel payloads is ending up at a higher IRQL, particularly at DISPATCH_LEVEL, which prevents us from accessing "Paged" memory etc. Some calls such as ZwQuerySystemInformation() will fail prematurly and BSOD with DRIVER_IRQL_LESS_THAN_OR_EQUAL, which is disastrous for us as operators. 

To solve this issue, I looked into an alternative method using an old "queue" system in Windows 2000. Whilst other methods have been documented, such as abusing [MSR LSTAR and Hijacking the Syscall Handler](https://zerosum0x0.blogspot.com/2019/11/fixing-remote-windows-kernel-payloads-meltdown.html) - I find his newer method for kernels with the Meltdown patch to be applied to be unfortunately unstable - and needed a more unversal solution.

Luckily, Microsoft provides such as system via `ExQueueWorkItem` - By locating the location of ourselves in memory, and copying ourselves to a new NonPagedPool ( to avoid issues in case our original reference is freed after we return ) - we can queue an work item to be executed at PASSIVE_LEVEL from DISPATCH_LEVEL :D

The implementation is detailed in [irql.c](source/irql.c)

## Thread Hijacking From Kernel

For the lazy: I've chosen to search for an arbitrary thread within the specified process ( in my case: notepad.exe for testing ) - and then hijacking it using PsSetContextThread() / PsGetContextThread(). Some odd thing I noticed is that it seems to spawn from Etw / NtTraceControl(), at least when targeting Notepad. If anyone has any clue as to why this is, I'd be interested in hearing from you =). I've yet to look myself.
