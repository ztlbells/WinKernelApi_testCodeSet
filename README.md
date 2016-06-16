# WinKernelApi_testCodeSet

Host PC: Windows 10 + WDK 10 ( WinDbg x64 )

Virtual PC: Windows XP SP3 + WDK 7600

driver loader: SRVINSTW

*notify.c: Using callback function to notify the status of each process: created or destroyed.

*hideMDL.c: from rootkit.com. Hooking zwQuerySystemInformation(), hide a determined process( in this code, I hide wordpad.exe. ).In order to make sure that this process have enough time to run, this code adds running time into idle's time (both user mode and kernel mode). What should be emphasized is that this code, instead of using CR0 trick, creates a new MDL from non-paged pool rewrite.

*tranverse_pid.c: Although hideMDL can remove wordpad.exe from systemInfo, we can tranverse _LIST_ENTRY to know the exact running processes. But remember, the offsets, like EPROCESS to OBJECT_TABLE(_HANDLE_TABLE) and UniqueProcessId to OBJECT_TABLE, vary from OS to OS (ORZ...). So before using this code, please check the offset values in macro by using nt!~ in WinDbg. Fortunately, _LIST_ENTRY is a struct for a ring linkedlist, which is very easy to utilize! Again, take care of the offsets! 

*check_if_hidden_fin.c: If a process was hidden by hideMDL.sys, it can be detected by this code. Its output looks like:

...
No.0031 : pid-3868 [I AM HERE:)]

No.0032 : pid-4028 [HIDDEN:()]

No.0033 : pid-2808 [I AM HERE:)]

No.0034 : pid-2164 [I AM HERE:)]

No.0035 : pid-0000 [I AM HERE:)]

[END]:Detection finished


