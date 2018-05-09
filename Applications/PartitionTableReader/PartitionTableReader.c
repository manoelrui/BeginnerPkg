#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/BlockIo.h>


CHAR8 PARTITION_TABLE_SIGNATURE[] = {0x54, 0x52, 0x41, 0x50, 0x20, 0x49, 0x46, 0x45};

struct _GPT_HEADER {
    CHAR8 Signature[8];
    CHAR8 Revision[4];
    CHAR8 HeaderSize[4];
    CHAR8 HeaderCRC32[4];
    CHAR8 Reserved0[4];
    CHAR8 MyLBA[8];
    CHAR8 AlternateLBA[8];
    CHAR8 FirstUsableLBA[8];
    CHAR8 LastUsableLBA[8];
    CHAR8 DiskGUID[16];
    CHAR8 PartitionEntryLBA[8];
    CHAR8 NumberOfPartitionEntries[4];
    CHAR8 SizeOfPartitionEntry[4];
    CHAR8 PartitionEntryArrayCRC32[4];
};

typedef struct _GPT_HEADER GPT_HEADER;

CHAR8*
EFIAPI
SwapArray(
    IN CHAR8 *Buffer,
    IN UINTN BufferSize
    ) 
{
    for (UINTN It = 0; It < BufferSize / 2; It++) {
        CHAR8 Temp = Buffer[It];
        Buffer[It] = Buffer[BufferSize - It - 1];
        Buffer[BufferSize - It - 1] = Temp;
    }

    return Buffer;
}

VOID
EFIAPI
PrintBuffer (
    IN CHAR8 *Buffer,
    IN UINTN BufferSize
    ) 
{
    for (UINTN It = 0; It < BufferSize - 1; It++) {
        Print(L"%X ", Buffer[It]);
    }

    Print(L"%X", Buffer[BufferSize - 1]);
}

VOID
EFIAPI
PrintGPTHeader (
    IN  GPT_HEADER* Header
    ) 
{
    Print(L"Signature: ");
    PrintBuffer(Header->Signature, sizeof(Header->Signature));
    Print(L"\n");

    Print(L"Revision: ");
    PrintBuffer((CHAR8*) Header->Revision, sizeof(Header->Revision));
    Print(L"\n");

    Print(L"Header Size: ");
    PrintBuffer((CHAR8*) Header->HeaderSize, sizeof(Header->HeaderSize));
    Print(L"\n");

    Print(L"Header CRC32: ");
    PrintBuffer((CHAR8*) Header->HeaderCRC32, sizeof(Header->HeaderCRC32));
    Print(L"\n");

    Print(L"Reserved: ");
    PrintBuffer((CHAR8*) Header->Reserved0, sizeof(Header->Reserved0));
    Print(L"\n");

    Print(L"MyLBA: ");
    PrintBuffer((CHAR8*) Header->MyLBA, sizeof(Header->MyLBA));
    Print(L"\n");

    Print(L"AlternateLBA: ");
    PrintBuffer((CHAR8*) Header->AlternateLBA, sizeof(Header->AlternateLBA));
    Print(L"\n");

    Print(L"FirstUsableLBA: ");
    PrintBuffer((CHAR8*) Header->FirstUsableLBA, sizeof(Header->FirstUsableLBA));
    Print(L"\n");

    Print(L"LastUsableLBA: ");
    PrintBuffer((CHAR8*) Header->LastUsableLBA, sizeof(Header->LastUsableLBA));
    Print(L"\n");

    Print(L"DiskGUID: ");
    PrintBuffer((CHAR8*) Header->DiskGUID, sizeof(Header->DiskGUID));
    Print(L"\n");

    Print(L"PartitionEntryLBA: ");
    PrintBuffer((CHAR8*) Header->PartitionEntryLBA, sizeof(Header->PartitionEntryLBA));
    Print(L"\n");

    Print(L"NumberOfPartitionEntries: ");
    PrintBuffer((CHAR8*) Header->NumberOfPartitionEntries, sizeof(Header->NumberOfPartitionEntries));
    Print(L"\n");

    Print(L"SizeOfPartitionEntry: ");
    PrintBuffer((CHAR8*) Header->SizeOfPartitionEntry, sizeof(Header->SizeOfPartitionEntry));
    Print(L"\n");

    Print(L"PartitionEntryArrayCRC32: ");
    PrintBuffer((CHAR8*) Header->PartitionEntryArrayCRC32, sizeof(Header->PartitionEntryArrayCRC32));
    Print(L"\n");
}

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
    BOOLEAN GPTExists = FALSE;

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
 
        Status = BlockIo->ReadBlocks(
                BlockIo,
                BlockIo->Media->MediaId,
                (EFI_LBA) 1,
                BlockSize,
                BlockBuffer
                );

        if (Status == EFI_NO_MEDIA) {
            FreePool(BlockBuffer);
            BlockBuffer = NULL;

            continue;
        } else if (EFI_ERROR(Status)) {
            Print(L"Error to read blocks - %r\n", Status);
            goto FREE_RESOURCES;
        }

        GPT_HEADER *Header = (GPT_HEADER*) BlockBuffer;
        SwapArray(Header->Signature, sizeof(Header->Signature));
        if (CompareMem(Header->Signature, PARTITION_TABLE_SIGNATURE, sizeof(Header->Signature)) == 0) {
            Print(L"GPT Header:\n\n");
            PrintGPTHeader(Header);        
            Print(L"\n\n");

            GPTExists = TRUE;
            FreePool(BlockBuffer);
            BlockBuffer = NULL;    
        } else {

            FreePool(BlockBuffer);
            BlockBuffer = NULL;
            continue;
        }
    }

    if (!GPTExists) {
        Print(L"It was not found any GPT table.\n\n");
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
