//@file traversePID
//@date 2016.06.10

#include<ntddk.h>
#include<windef.h>

//some offset params in win xp sp3
//which can be checked in kd>..(e.g. using nt!_EPROCESS and nt!_HANDLE_TABLE in windbg)
#define EPROCESS_OBJECT_TABLE_OFFESET 		0x0c4
#define OBJECT_TABLE_LIST_ENTRY_OFFESET     0x01c
#define OBJECT_TABLE_PID_OFFSET			  	0x008

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
void getAllProc(){
	PEPROCESS eproc;
	BYTE* start;
	BYTE* addr;
	DWORD pid;
	DWORD nproc;

	nproc = 1;
	eproc = PsGetCurrentProcess();
	addr = (BYTE*)eproc + EPROCESS_OBJECT_TABLE_OFFESET;

	//start -> &object_table
	start = (BYTE*)(*((DWORD*)addr));
	pid = getPID(start);\
	DbgPrint("No.%04d : pid-%04d\n",nproc,pid);

	addr=getNextEntry(start,OBJECT_TABLE_LIST_ENTRY_OFFESET);	

	while(addr!=start){
		pid = getPID(addr);
		nproc++;
		DbgPrint("No.%04d : pid-%04d\n",nproc,pid);
		addr=getNextEntry(addr,OBJECT_TABLE_LIST_ENTRY_OFFESET);
	}
	
}

VOID DriverUnload(PDRIVER_OBJECT driver){
	DbgPrint("[END]:traversePID...\r\n");
}

//entry == main
NTSTATUS DriverEntry (PDRIVER_OBJECT driver, PUNICODE_STRING reg_path){

	//unload
	driver->DriverUnload = DriverUnload;

	DbgPrint("[BEGIN]:traversePID...\r\n");
	getAllProc();
	return STATUS_SUCCESS;
}