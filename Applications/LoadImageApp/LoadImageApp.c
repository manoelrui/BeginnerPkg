#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/DebugLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/BlockIo.h>
#include <Library/DevicePathLib.h>


EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    UINTN NumHandles;
    UINTN Index;
    EFI_HANDLE *SFS_Handles;
    EFI_HANDLE AppImageHandle;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    CONST CHAR16* FileName = L"HelloUEFI.efi";
    EFI_DEVICE_PATH_PROTOCOL* FilePath;
    

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            &NumHandles,
            &SFS_Handles);

    if (Status != EFI_SUCCESS) {
        Print(L"Could not find handles - %r\n", Status);
        return Status;
    }

    for (Index = 0; Index < NumHandles; Index++) {
        Status = gBS->HandleProtocol(
                SFS_Handles[Index],
                &gEfiSimpleFileSystemProtocolGuid,
                (VOID**) &BlkIo
                );
    
        if (Status != EFI_SUCCESS) {
            Print(L"Protocol is not supported - %r\n", Status);
            return Status;
        }
   
        FilePath = FileDevicePath(SFS_Handles[Index], FileName);
        Status = gBS->LoadImage(
                TRUE, 
                ImageHandle, 
                FilePath, 
                NULL, 
                0, 
                &AppImageHandle);

        if (Status != EFI_SUCCESS) {
            Print(L"Could not load the image - %r\n", Status);
            continue;
        }

        Print(L"Loaded the image with success\n");

        Status = gBS->StartImage(AppImageHandle, NULL, NULL);
        if (Status != EFI_SUCCESS) {
            Print(L"Could not start the image - %r\n", Status);
            continue;
        }
    }

    return Status;
}

