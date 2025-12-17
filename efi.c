#include "efi.h"

#include <stdint.h>

// EFI Image Entry Point
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
                           EFI_SYSTEM_TABLE* SystemTable) {
  (void)ImageHandle;  // Prevent compiler warning

  // Set text to yellow fg/ green bg
  SystemTable->ConOut->SetAttribute(SystemTable->ConOut,
                                    EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK));

  // Clear screen to bg color
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                    u"Welcome to UEFI ResearchOS!\r\n");

  SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                    u"Press any enter to shutdown...");

  // Wait until keypress, then return
  EFI_INPUT_KEY key;
  while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) !=
         EFI_SUCCESS);
  SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0,
                                            NULL);

  // Should never get here
  return EFI_SUCCESS;
}
