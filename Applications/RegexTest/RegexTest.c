#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/RegularExpressionProtocol.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
	EFI_REGEX_SYNTAX_TYPE SyntaxTypes[] =  {
		EFI_REGEX_SYNTAX_TYPE_POSIX_EXTENDED_GUID,
		EFI_REGEX_SYNTAX_TYPE_PERL_GUID,
		EFI_REGEX_SYNTAX_TYPE_ECMA_262_GUID
	};
	CHAR16 *SyntaxTypesStr[] = {
		L"EFI_REGEX_SYNTAX_TYPE_POSIX_EXTENDED_GUID",
		L"EFI_REGEX_SYNTAX_TYPE_PERL_GUID",
		L"EFI_REGEX_SYNTAX_TYPE_ECMA_262_GUID"
	};
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN NumHandles = 0;
	EFI_HANDLE *RegexHandle = NULL;
	EFI_REGULAR_EXPRESSION_PROTOCOL *RegexProtocol = NULL;
	EFI_REGEX_SYNTAX_TYPE	RegExSyntaxTypeList[100];
	UINTN RegExSyntaxTypeListSize = ARRAY_SIZE(RegExSyntaxTypeList);
	INT8 i = 0;
	INT8 j = 0;
	BOOLEAN Res = FALSE;
	UINTN CapturesCount = 0;
	EFI_REGEX_CAPTURE* Captures = NULL;


	Status = gBS->LocateHandleBuffer(
				ByProtocol,
				&gEfiRegularExpressionProtocolGuid,
				NULL,
				&NumHandles,
				&RegexHandle
				);

	if(EFI_ERROR(Status)) {
		DEBUG ((DEBUG_ERROR, "ERROR: Could not locate Regular Expression handle (Err:%r)\n", Status));
		return Status;
	}

	Status = gBS->OpenProtocol(
			*RegexHandle,
			&gEfiRegularExpressionProtocolGuid,
			(VOID**) &RegexProtocol,
			ImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);

	if(EFI_ERROR(Status)) {
		DEBUG ((DEBUG_ERROR, "ERROR: Could not open the Regular Expression Protocol (Err:%r)\n", Status));
		return Status;
	}

	Status = RegexProtocol->GetInfo(
			RegexProtocol,
			&RegExSyntaxTypeListSize,
			RegExSyntaxTypeList
			);

	if(EFI_ERROR(Status)) {
		DEBUG ((DEBUG_ERROR, "ERROR: Regex GetInfo fail (Err:%r)\n", Status));
		return Status;
	}

	Print(L"Size of systax type list: %d\n", RegExSyntaxTypeListSize);


	for(i = 0; i < RegExSyntaxTypeListSize; i++) {
		for(j = 0; j < ARRAY_SIZE(SyntaxTypes); j++) {
			if(CompareMem(&RegExSyntaxTypeList[i], &SyntaxTypes[j], ARRAY_SIZE(SyntaxTypes)) == 0) {
				Print(L"Syntax supported: %s\n", SyntaxTypesStr[j]);
			}
		}
	}

	Status = RegexProtocol->MatchString(
			RegexProtocol,
			L"zzzzaffffffffb",
			L"a(.*)b|[e-f]+",
			NULL,
			&Res,
			&Captures,
			&CapturesCount
		);

	if(EFI_ERROR(Status)) {
		DEBUG ((DEBUG_ERROR, "ERROR: Match string failed (Err:%r)\n", Status));
		return Status;
	}

	Print(L"\n");
	Print(L"Captures count: %d\n", CapturesCount);
	for(i = 0; i < CapturesCount; i++) {
		CHAR16 *StrOut = (CHAR16*) AllocateZeroPool((Captures[i].Length + 1) * sizeof(CHAR16));
		CopyMem(StrOut, Captures[i].CapturePtr, Captures[i].Length * sizeof(CHAR16));
		StrOut[Captures[i].Length] = '\0';
		Print(L"Str output [%d]: %s - Length: %d\n", i, StrOut, Captures[i].Length);
		FreePool(StrOut);
	}

  return Status;
}
