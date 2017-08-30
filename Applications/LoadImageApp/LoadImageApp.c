#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Library/DevicePathLib.h>
#include <Protocol/LoadedImage.h>

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
    EFI_HANDLE AppImageHandle = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    CONST CHAR16 *FileName = L"HelloImageEntryPoint.efi";
    EFI_DEVICE_PATH_PROTOCOL *FilePath;
    EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
    UINTN ExitDataSize;


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
        Status = gBS->OpenProtocol(
                SFS_Handles[Index],
                &gEfiSimpleFileSystemProtocolGuid,
                (VOID**) &BlkIo,
                ImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
    
        if (Status != EFI_SUCCESS) {
            Print(L"Protocol is not supported - %r\n", Status);
            return Status;
        }
   
        FilePath = FileDevicePath(SFS_Handles[Index], FileName);
        Status = gBS->LoadImage(
                FALSE, 
                ImageHandle, 
                FilePath, 
                (VOID*) NULL, 
                0, 
                &AppImageHandle);

        if (Status != EFI_SUCCESS) {
            Print(L"Could not load the image - %r\n", Status);
            continue;
        }

        Print(L"Loaded the image with success\n");
        Status = gBS->OpenProtocol(
                AppImageHandle,
                &gEfiLoadedImageProtocolGuid,
                (VOID**) &ImageInfo,
                ImageHandle,
                (VOID*) NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );

        Print(L"ImageInfo opened\n");


        if (!EFI_ERROR(Status)) {
            Print(L"ImageSize = %d\n", ImageInfo->ImageSize);
        }

        Print(L"Image start:\n");
        Status = gBS->StartImage(AppImageHandle, &ExitDataSize, (CHAR16**) NULL);
        if (Status != EFI_SUCCESS) {
            Print(L"Could not start the image - %r %x\n", Status, Status);
            Print(L"Exit data size: %d\n", ExitDataSize);
            continue;
        }
    }

    return Status;
}

