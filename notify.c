//@file notify.c
//@date 2016.06.08

#include<ntifs.h>
#include<ntddk.h>


VOID ProcessCallBack(IN HANDLE ParentId,IN HANDLE  ProcessId,IN BOOLEAN  bCreate);
VOID UnloadDriver(PDRIVER_OBJECT DriverObject);

//NTSTATUS PsSetCreateProcessNotifyRoutine(IN PCREATE_PROCESS_NOTIFY_ROUTINE  NotifyRoutine,IN BOOLEAN  Remove);
//remove: true=delete , false=add

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegisterPath)
{
    NTSTATUS Status = STATUS_SUCCESS;
    DriverObject->DriverUnload = UnloadDriver;
    Status  = PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)ProcessCallBack,FALSE);
    return STATUS_SUCCESS;
}

VOID ProcessCallBack(IN HANDLE ParentId,IN HANDLE  ProcessId,IN BOOLEAN  bCreate)
{
    if (bCreate==TRUE)
    {
        DbgPrint("[CALLBACK]:Proc No.%d created\r\n",ProcessId); 
    }
    else
    {
        DbgPrint("[CALLBACK]:Proc No.%d destroyed\r\n",ProcessId); 
    }
}


VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
    PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)ProcessCallBack,TRUE);
    DbgPrint("unloading driver\r\n");
}