                                        MCISPC.DRV
                                      by Marius Fodor
                                  (marius.fodor@usa.net)
                                    

	This driver is pretty much just an interface wrapper for SPC700EMU.DLL. It does not contain
any comments, so you will have to do your best in deciphering its workings. In order to compile
it you will need a compiler that can make 16-bit DLLs, such as Microsoft Visual C++ 1.52 or below,
Microsoft took out all the 16-bit support out of later versions of Visual C++, but if you own a
newer version of Visual C++ you can call up Microsoft and they will send you a free copy of Visual
C++ 1.52. You will also need a header file from the Microsoft Driver Development Kit (DDK) named
MMDDK.H. You will also need the thunk compiler (THUNK.EXE) also from the DDK, to compile the thunk
script (THK0.THK).

	This driver uses what is called a flat thunk to communicate with SPC700EMU.DLL, this feature
is not supported under Windows NT (or Windows 2000). I don't have Windows NT, so it would be nice
if somebody could port it to Windows NT.

	Well, anyway, drop me a line if you have any questions. Bye.