//@file check_if_hidden
//@date 2016.06.10

#include<ntddk.h>
#include<windef.h>

//some offset params in win xp sp3
//which can be checked in kd>..(e.g. using nt!_EPROCESS and nt!_HANDLE_TABLE in windbg)
#define EPROCESS_OBJECT_TABLE_OFFESET 		0x0c4
#define OBJECT_TABLE_LIST_ENTRY_OFFESET     0x01c
#define OBJECT_TABLE_PID_OFFSET			  	0x008

//align in 1-bit
#pragma pack(1)
typedef struct ServiceDescriptorEntry {
        unsigned int *ServiceTableBase;
        unsigned int *ServiceCounterTableBase; 
        unsigned int NumberOfServices;
        unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

__declspec(dllimport)  ServiceDescriptorTableEntry_t KeServiceDescriptorTable;
#define SYSTEMSERVICE(_function)  KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]

NTSYSAPI
NTSTATUS
NTAPI ZwQuerySystemInformation(
            IN ULONG SystemInformationClass,
                        IN PVOID SystemInformation,
                        IN ULONG SystemInformationLength,
                        OUT PULONG ReturnLength);


//some used struct def 
typedef struct _SYSTEM_THREAD_INFORMATION {    
    LARGE_INTEGER           KernelTime;    
    LARGE_INTEGER           UserTime;    
    LARGE_INTEGER           CreateTime;    
    ULONG                   WaitTime;    
    PVOID                   StartAddress;    
    CLIENT_ID               ClientId;    
    KPRIORITY               Priority;    
    LONG                    BasePriority;    
    ULONG                   ContextSwitchCount;    
    ULONG                   State;    
    KWAIT_REASON            WaitReason;    
}SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;    
    
typedef struct _SYSTEM_PROCESS_INFORMATION {    
    ULONG                   NextEntryOffset;    
    ULONG                   NumberOfThreads;    
    LARGE_INTEGER           Reserved[3];    
    LARGE_INTEGER           CreateTime;    
    LARGE_INTEGER           UserTime;    
    LARGE_INTEGER           KernelTime;    
    UNICODE_STRING          ImageName;    
    KPRIORITY               BasePriority;    
    HANDLE                  ProcessId;    
    HANDLE                  InheritedFromProcessId;    
    ULONG                   HandleCount;    
    ULONG                   Reserved2[2];    
    ULONG                   PrivatePageCount;    
    VM_COUNTERS             VirtualMemoryCounters;    
    IO_COUNTERS             IoCounters;    
    SYSTEM_THREAD_INFORMATION           Threads[0];    
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;    
   
DWORD getPID(BYTE* curr){
	// byte ptr -> eproc
	DWORD *pid_ptr;
	DWORD pid;
	pid_ptr = (DWORD*)(curr + OBJECT_TABLE_PID_OFFSET);
	pid = *pid_ptr;
	return pid;
}

BYTE* getNextEntry(BYTE* prev, DWORD offset){
	//in struct _LIST_ENTRY/*PLIST_ENTRY, FLink points to next _LIST_ENTRY
	PLIST_ENTRY list_entry = NULL;
	PLIST_ENTRY f_list_entry = NULL;
	BYTE* entry;

	entry = prev + offset;	
	list_entry = (PLIST_ENTRY) (prev + offset);
	f_list_entry = list_entry -> Flink;

	entry = (BYTE*)f_list_entry;
	return entry - offset;
}

//if hidden, return 0
BYTE checkExistence(DWORD pid){
	 NTSTATUS ntStatus;
	 ULONG BufferSize; 
	 //buffer
	 PVOID pBuffer=NULL;
	 PSYSTEM_PROCESS_INFORMATION SystemInfo_ptr; 
	 BYTE flag; 

   ntStatus = ZwQuerySystemInformation (
   					//when SystemInformationClass == 5 , we can get SystemProcessInformation
					0x05,
					NULL,
					0,
					&BufferSize );

   //get space for buffer
   pBuffer=ExAllocatePoolWithTag(NonPagedPool,BufferSize,'buf');


   //get proc info
   ntStatus=ZwQuerySystemInformation(0x05,pBuffer,BufferSize,NULL);

   SystemInfo_ptr=(PSYSTEM_PROCESS_INFORMATION)pBuffer;

   //tranverse sysInfo
   flag= 0;
   while (TRUE)                   
    {                       
            if (SystemInfo_ptr->ProcessId == pid){
            	//matched
            	flag = 1;
            	break;
            }
		//end of linkedList
        if (0 == SystemInfo_ptr->NextEntryOffset)
        {  
            break;  
        }  
        //next one
        SystemInfo_ptr=(PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)SystemInfo_ptr) + SystemInfo_ptr->NextEntryOffset);
    }  
    return flag;
}
//check if hidden func
void checkIfHidden(){
	//using getAllProc() from tranverse_pid.c to tranverse all of the processes
	//while tranversing, checking the existence of each process in sysinfoclass
	PEPROCESS eproc;
	BYTE* start;
	BYTE* addr;
	DWORD pid;
	DWORD nproc;
	BYTE exi_flag;

	nproc = 1;
	eproc = PsGetCurrentProcess();
	addr = (BYTE*)eproc + EPROCESS_OBJECT_TABLE_OFFESET;

	//start -> &object_table
	start = (BYTE*)(*((DWORD*)addr));
	pid = getPID(start);
	DbgPrint("No.%04d : pid-%04d ",nproc,pid);

	exi_flag=checkExistence(pid);
	if(exi_flag==0) DbgPrint("[HIDDEN:()]\n");
	else DbgPrint("[I AM HERE:)]\n");

	addr=getNextEntry(start,OBJECT_TABLE_LIST_ENTRY_OFFESET);	

	while(addr!=start){
		pid = getPID(addr);
		nproc++;
		DbgPrint("No.%04d : pid-%04d ",nproc,pid);
		exi_flag=checkExistence(pid);
		if(exi_flag==0) DbgPrint("[HIDDEN:()]\n");
		else DbgPrint("[I AM HERE:)]\n");
		addr=getNextEntry(addr,OBJECT_TABLE_LIST_ENTRY_OFFESET);
	}
}



VOID DriverUnload(PDRIVER_OBJECT driver){
	DbgPrint("[END]:Detection finished\r\n");
}

//entry == main
NTSTATUS DriverEntry (PDRIVER_OBJECT driver, PUNICODE_STRING reg_path){

	//unload
	driver->DriverUnload = DriverUnload;

	DbgPrint("[BEGIN]:Detecting hidden processes...\r\n");
	checkIfHidden();
	return STATUS_SUCCESS;
}