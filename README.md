# WinKernelApi_testCodeSet

Host PC: Windows 10 + WDK 10 ( WinDbg x64 )

Virtual PC: Windows XP SP3 + WDK 7600

driver loader: SRVINSTW

notify.c: Using callback function to notify the status of each process: created or destroyed.

hideMDL.c: from rootkit.com. Hooking zwQuerySystemInformation(), hide a determined process( in this code, I hide wordpad.exe. ).In order to make sure that this process have enough time to run, this code adds running time into idle's time (both user mode and kernel mode). What should be emphasized is that this code, instead of using CR0 trick, it creates a new MDL from non-paged pool rewrite.

tranverse_pid.c: Although hideMDL can remove wordpad.exe from systemInfo, we can tranverse _LINK_ENTRY to know the exact running processes. But remember, the offsets, like EPROCESS to OBJECT_TABLE(_HANDLE_TABLE) and UniqueProcessId to OBJECT_TABLE, vary from OS to OS (ORZ...). Fortunately, _LINK_ENTRY is a struct for a ring linkedlist, which is very easy to utilize! Again, take care of the offsets! 
