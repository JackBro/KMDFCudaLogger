/*++

Module Name:

driver.c

Abstract:

This file contains the driver entry points and callbacks.

Environment:

Kernel-mode Driver Framework


look at IoEnumerateDeviceObjectList 

--*/

#include "Driver.h"
#include "ControlDevice.h"
#include <AccessBus.h>
#include <initguid.h>
#include <wdmguid.h>
//#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, KMDFMemoryDumpEvtDeviceAdd)
#pragma alloc_text (PAGE, KMDFMemoryDumpEvtDriverContextCleanup)
#endif


NTSTATUS GetPciDeviceByName(_In_ PDRIVER_OBJECT  pDriverObject, PCWSTR driverName) {
	NTSTATUS status;
	POBJECT_TYPE driverObjectType;
	UNICODE_STRING pciDriverName;

	RtlInitUnicodeString(&pciDriverName, driverName);
	driverObjectType = ObGetObjectType(pDriverObject);
	status = ObReferenceObjectByName(&pciDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, driverObjectType, //(POBJECT_TYPE)IoDriverObjectType,
		KernelMode, NULL, (PVOID*)&pciDriverObject);

	if (NT_SUCCESS(status)) {

		KdPrint(("pciDriverObject is [0x%lx]\n", pciDriverObject));
		pciVideoDeviceObject = pciDriverObject->DeviceObject;
		while (pciVideoDeviceObject->DeviceType != 0x23 && pciVideoDeviceObject->NextDevice) { //     pciVideoDeviceObject->Flags != 0x2004
			pciVideoDeviceObject = pciVideoDeviceObject->NextDevice;
		}


			/** /

			PCI_COMMON_CONFIG commonConfig;
			PCI_COMMON_HEADER PciHeader;
			PPCI_COMMON_CONFIG PciConfig = (PPCI_COMMON_CONFIG)&PciHeader;
			KdPrint(("ReadConfigSpace: Type was %d\n", PciHeader.HeaderType));

			// PciConfig = (PPCI_COMMON_HEADER)ExAllocatePoolWithTag(NonPagedPool, sizeof(PCI_COMMON_HEADER), 'evol');

			ReadConfigSpace(pciVideoDeviceObject, PciConfig, 0, sizeof(PCI_COMMON_HEADER));

			if (PciConfig) {
				KdPrint(("ReadConfigSpace: Type %d\n", PciHeader.HeaderType));

				Bar0Address = PciHeader.u.type0.BaseAddresses[0];
				HostMemAddress = Bar0Address +   0x1700;
				WindowAddress = Bar0Address + 0x700000;
				KdPrint(("   Base Address 0   [0x%lx]\n", Bar0Address));
				KdPrint(("   Host Mem Address [0x%lx]\n", HostMemAddress));
				KdPrint(("   Window Address   [0x%lx]\n\n", WindowAddress));

				/** /
				WindowPointer = WindowAddress;
				KdPrint(("   Window Pointer   [0x%lx] [0x%lx]\n\n", WindowPointer, *WindowPointer));
				/** /

				KdPrint(("pciDriverObject is [0x%lx]\n"));
			}
			else {
				KdPrint(("ReadConfigSpace failed :-( \n"));
			}
			/**/


	}
	return status;
}

NTSTATUS GetPciDevice(_In_ PDRIVER_OBJECT  pDriverObject) {
	return GetPciDeviceByName(pDriverObject, L"\\Driver\\pci");

	/** /
	NTSTATUS status;
	POBJECT_TYPE driverObjectType;
	UNICODE_STRING pciDriverName;

	RtlInitUnicodeString(&pciDriverName, L"\\Driver\\pci");
	driverObjectType = ObGetObjectType(pDriverObject);
	status = ObReferenceObjectByName(&pciDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, driverObjectType, //(POBJECT_TYPE)IoDriverObjectType,
		KernelMode, NULL, (PVOID*)&pciDriverObject);

	if (NT_SUCCESS(status)) {

		KdPrint(("pciDriverObject is [0x%lx]\n", pciDriverObject));
		pciVideoDeviceObject = pciDriverObject->DeviceObject;
		while (pciVideoDeviceObject->DeviceType != 0x23 && pciVideoDeviceObject->NextDevice) { //     pciVideoDeviceObject->Flags != 0x2004
			pciVideoDeviceObject = pciVideoDeviceObject->NextDevice;
		}
		if (pciVideoDeviceObject) {
			KdPrint(("pciVideoDeviceObject is [0x%lx]\n", pciVideoDeviceObject));
		}

	}
	return status;
	/**/
}



NTSTATUS
DriverEntry(
_In_ PDRIVER_OBJECT  DriverObject,
_In_ PUNICODE_STRING RegistryPath
)
{
	WDF_DRIVER_CONFIG config;
	NTSTATUS status = STATUS_SUCCESS;
	WDF_OBJECT_ATTRIBUTES attributes;
	KdPrint(("KMDF Memory Dump - Driver Entry\n"));
	GetPciDevice(DriverObject);
	KdPrint(("Creating Control Device\n"));
	status = CreateControlDevice(DriverObject, RegistryPath);

	return status;
}

NTSTATUS
KMDFMemoryDumpEvtDeviceAdd(
_In_    WDFDRIVER       Driver,
_Inout_ PWDFDEVICE_INIT DeviceInit
)
/*++
Routine Description:

EvtDeviceAdd is called by the framework in response to AddDevice
call from the PnP manager. We create and initialize a device object to
represent a new instance of the device.

Arguments:

Driver - Handle to a framework driver object created in DriverEntry

DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

NTSTATUS

--*/
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(DeviceInit);
	UNREFERENCED_PARAMETER(Driver);

	PAGED_CODE();

	//    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	status = STATUS_SUCCESS ;// KMDFMemoryDumpCreateDevice(DeviceInit);

	//    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	return status;
}

VOID
KMDFMemoryDumpEvtDriverContextCleanup(
_In_ WDFOBJECT DriverObject
)
/*++
Routine Description:

Free all the resources allocated in DriverEntry.

Arguments:

DriverObject - handle to a WDF Driver object.

Return Value:

VOID.

--*/
{
	UNREFERENCED_PARAMETER(DriverObject);

	PAGED_CODE();

	//   TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//
	// Stop WPP Tracing
	//
	//    WPP_CLEANUP( WdfDriverWdmGetDriverObject(DriverObject) );

}
