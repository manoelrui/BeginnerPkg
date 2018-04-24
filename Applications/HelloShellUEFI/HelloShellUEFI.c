#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>


INTN
EFIAPI
ShellAppMain (
  IN UINTN  Argc,
  IN CHAR16 **Argv
  )
{
  Print(L"Hello UEFI.\n");
  Print(L"We are using the shell entry point.\n");
  Print(L"This is the world of EDK II.\n");

  return(0);
}
