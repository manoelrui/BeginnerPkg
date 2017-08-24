#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>

/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  Print(L"Hello there fellow Programmer.\n");
  Print(L"Welcome to the world of EDK II.\n");

  return(0);
}
