#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
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
    UINTN BufferSize = 0;
    CHAR8 *BlockBuffer = NULL;
    UINTN BlockSize = 0;

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiBlockIoProtocolGuid,
            NULL,
            &BufferSize,
            &Handles
            );

    if (EFI_ERROR(Status)) {
        Print(L"Error to locate handle\n");
        goto FREE_RESOURCES;
    }

    if (BufferSize < 1) {
        Print(L"There is not any Block IO Protocol\n");
        goto FREE_RESOURCES;
    }

    Status = gBS->OpenProtocol(
            Handles[0],
            &gEfiBlockIoProtocolGuid,
            (VOID**) &BlockIo,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL  
            );

    if (EFI_ERROR(Status)) {
        Print(L"Error to open Block IO protocol\n");
        goto FREE_RESOURCES;
    }

    BlockSize = BlockIo->Media->BlockSize;
    BlockBuffer = AllocateZeroPool(BlockSize);

    Print(L"Block Size: %d\n", BlockSize);

    Status = BlockIo->ReadBlocks(
            BlockIo,
            BlockIo->Media->MediaId,
            (EFI_LBA) 0,
            BlockSize,
            BlockBuffer
            );

    if (EFI_ERROR(Status)) {
        Print(L"Error to read blocks\n");
        goto FREE_RESOURCES;
    }

    Print(L"Block #0 of first device found:\n");
    for (UINTN It = 0; It < BlockSize / sizeof(CHAR8); It++) {
        Print(L"%x", BlockBuffer[0]);
    }
    Print(L"\n");

FREE_RESOURCES:

    if (Handles != NULL) {
        FreePool(Handles);
    }

    if (BlockBuffer != NULL) {
        FreePool(BlockBuffer);
    }

    return EFI_SUCCESS;
}
