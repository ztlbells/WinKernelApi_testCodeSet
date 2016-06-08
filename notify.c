#include<ntifs.h>
#include<ntddk.h>


VOID ProcessCallBack(IN HANDLE ParentId,IN HANDLE  ProcessId,IN BOOLEAN  bCreate);
VOID UnloadDriver(PDRIVER_OBJECT DriverObject);

//NTSTATUS PsSetCreateProcessNotifyRoutine(IN PCREATE_PROCESS_NOTIFY_ROUTINE  NotifyRoutine,IN BOOLEAN  Remove);
//remove: true=delete , false=add

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegisterPath)
{
    NTSTATUS Status = STATUS_SUCCESS;
    DbgPrint("loading driver\r\n");
    DriverObject->DriverUnload = UnloadDriver;
    Status  = PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)ProcessCallBack,FALSE);
    return STATUS_SUCCESS;
}

VOID ProcessCallBack(IN HANDLE ParentId,IN HANDLE  ProcessId,IN BOOLEAN  bCreate)
{
    if (bCreate==TRUE)
    {
        DbgPrint("No.%d process created\r\n",ProcessId); 
    }
    else
    {
        DbgPrint("No.%d process destroyed\r\n",ProcessId); 
    }
}


VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
    PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)ProcessCallBack,TRUE);
    DbgPrint("unloading driver\r\n");
}