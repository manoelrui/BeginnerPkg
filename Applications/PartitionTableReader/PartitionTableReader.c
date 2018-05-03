#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/BlockIo.h>


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE *Handles = NULL;
    EFI_BLOCK_IO_PROTOCOL *BlockIo = NULL;
    UINTN NumHandles = 0;
    CHAR8 *BlockBuffer = NULL;
    UINTN BlockSize = 0;

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiBlockIoProtocolGuid,
            NULL,
            &NumHandles,
            &Handles
            );

    if (EFI_ERROR(Status)) {
        Print(L"Error to locate handle - %r\n", Status);
        goto FREE_RESOURCES;
    }

    if (NumHandles < 1) {
        Print(L"There is not any Block IO Protocol - %r\n", Status);
        goto FREE_RESOURCES;
    }

    for (UINTN Index = 0; Index < NumHandles; Index++) {

        Status = gBS->OpenProtocol(
                Handles[Index],
                &gEfiBlockIoProtocolGuid,
                (VOID**) &BlockIo,
                ImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL  
                );

        if (EFI_ERROR(Status)) {
            Print(L"Error to open Block IO protocol - %r\n", Status);
            goto FREE_RESOURCES;
        }

        BlockSize = BlockIo->Media->BlockSize;
        BlockBuffer = AllocateZeroPool(BlockSize);

        Print(L"Block Size: %d\n", BlockSize);
        Print(L"Media ID: %d\n", BlockIo->Media->MediaId);
        Status = BlockIo->ReadBlocks(
                BlockIo,
                BlockIo->Media->MediaId,
                (EFI_LBA) 0,
                BlockSize,
                BlockBuffer
                );

        if (EFI_ERROR(Status)) {
            Print(L"Error to read blocks - %r\n", Status);
            goto FREE_RESOURCES;
        }

        DumpHex(0, 0, BlockSize, BlockBuffer);

        FreePool(BlockBuffer);
        BlockBuffer = NULL;

    }

FREE_RESOURCES:

    if (Handles != NULL) {
        FreePool(Handles);
    }

    if (BlockBuffer != NULL) {
        FreePool(BlockBuffer);
    }

    return EFI_SUCCESS;

}
