                                     MCISPCDLG.DLL
                                    by Marius Fodor
                                  (marius.fodor@usa.net)


	This DLL is used by the MCISPC.DRV driver to display its configuration dialog box. It's
needed because in order for Windows to do the flat thunking between MCISPC.DRV and SPC700EMU.DLL
you need to mark MCISPC.DRV as a Windows 4.0 DLL, but in doing so that makes all the resouces in
it inaccessible, such as dialog box templates. So I had to resort to this. I used Microsoft
Visual C++ 6.0 to compile it, but any 32-bit Windows compiler should do. There aren't any comments
in the source.

	Well, anyway, drop me a line if you have any questions. Bye.