
#ifndef HASHES_H
#define HASHES_H

//! Precalculated DJB2 Hashes.
#define H_KERNEL32                   0x6ddb9555
#define H_NTOSKRNL                   0xa3ad0390
#define H_NTDLL                      0x1edab0ed

#define H_EXFREEPOOL                 0x3f7747de
#define H_EXALLOCATEPOOL             0xa1fe8ce1
#define H_EXQUEUEWORKITEM            0xd6b8d919
#define H_PSRESUMEPROCESS            0x924633f8
#define H_KEGETCURRENTIRQL           0xee1c9930
#define H_PSSUSPENDPROCESS           0xc4464249
#define H_IOTHREADTOPROCESS          0xdb00c717
#define H_PSSETCONTEXTTHREAD         0x48f32151
#define H_PSGETCONTEXTTHREAD         0xb6755ac5
#define H_ZWFREEVIRTUALMEMORY        0x3c81f778
#define H_OBDEREFERENCEOBJECT        0x3de33965
#define H_KESTACKATTACHPROCESS       0x743362bf
#define H_KEUNSTACKDETACHPROCESS     0xcf8145d6
#define H_ZWALLOCATEVIRTUALMEMORY    0xb20c09db
#define H_ZWQUERYSYSTEMINFORMATION   0x8754a7f7
#define H_PSLOOKUPTHREADBYTHREADID   0x5eb140fa
#define H_PSGETPROCESSIMAGEFILENAME  0x73980d6b
#define H_PSLOOKUPPROCESSBYPROCESSID 0x0009b1c8

#endif // END HASHES_H
