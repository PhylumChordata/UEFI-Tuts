// UEFI From Scratch Tutorials - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/UEFI-Tuts

#include "efi.h"
#include "ErrorCodes.h"
#include "clibs.h"
#include "efilibs.h"

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
    
    ResetScreen();
    
    SetColor(EFI_WHITE);
    SetTextPosition(3, 2);
    Print(L"Welcome to the UEFI From Scratch Tutorials");
    
    SetColor(EFI_GREEN);
    SetTextPosition(8, 4);
    Print(L"Hit Any Key to write to a file\r\n");

    HitAnyKey();

    ClearScreen();
	
    // Writing to a file
	InitializeFILESYSTEM();
	
    WriteToFile("This is our file.\nWelcome youtubers !!!", u"test.txt");

    SetColor(EFI_GREEN);
    SetTextPosition(8, 10);
    Print(L"Hit q to quit | Hit r to reboot");

    ResetKeyboard();

    while(1)
    {
		Delay1();
		
		EFI_STATUS Status = CheckKey();
		if(Status == EFI_SUCCESS)
		{
			if(GetKey('q') == 1)
			{
				SHUTDOWN();
				break;
			}
			if(GetKey('r') == 1)
			{
				COLD_REBOOT();
				break;
			}
		}

	}

    // We should not make it to this point.
    COLD_REBOOT();

    // We should not make it to this point.
    return EFI_SUCCESS;
}
