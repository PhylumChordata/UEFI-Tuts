// UEFI From Scratch Tutorials - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/UEFI-Tuts

#ifndef EFILIBS_H
#define EFILIBS_H

#include <clibs.h>
#include <stdlib.h>
#include <string.h>

             // ARGB             
#define ORANGE 0xffffa500
#define CYAN   0xff00ffff
#define RED    0xffff0000
#define GREEN  0xff00ff00
#define BLUE   0xff0000ff
#define GRAY   0xff888888
#define WHITE  0xffffffff
#define BLACK  0xff000000

EFI_HANDLE ImageHandle;
EFI_SYSTEM_TABLE* SystemTable;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL GraphicsColor;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
EFI_FILE_PROTOCOL* RootFS;
void* OSBuffer_Handle;

void SetTextPosition(UINT32 Col, UINT32 Row)
{
    // Sets the Column and Row of the text screen cursor position.
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, Col, Row);
}

void ResetScreen()
{
    // This resets the whole console ( A.K.A. your display screen ) interface.
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
}

void ClearScreen()
{
    // This clears the screen buffer, but does not reset it.
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
}

void SetColor(UINTN Attribute)
{
    // We set the color for the text output.
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, Attribute);
}

void Print(CHAR16* str)
{
    // Text Output  ( A.K.A. ConOut is Console Out )
    SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

void HitAnyKey()
{
    // This clears the keyboard buffer.
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);

    // We setup the struct to take keyboard input.
    EFI_INPUT_KEY Key;

    // In a while loop to see if the keyboard has a key stroke in the buffer.
    while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
}

void ResetKeyboard()
{
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);
}

EFI_INPUT_KEY CheckKeystroke;
BOOLEAN GetKey(CHAR16 key)
{
    if(CheckKeystroke.UnicodeChar == key)
    {
        return 1;
    } else {
        return 0;
    }
}

EFI_STATUS CheckKey()
{
    return SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &CheckKeystroke);
}

// This sets the color of the pixels ( Graphics Color )
void SetGraphicsColor(UINT32 color)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GColor;
    GColor.Reserved = color >> 24;
    GColor.Red      = color >> 16;
    GColor.Green    = color >> 8;
    GColor.Blue     = color;
    GraphicsColor = GColor;
}

// This positions the pixel in the row and column ( X and Y )
void SetPixel(UINT32 xPos, UINT32 yPos)
{
    // TODO : Add in a choice instead of defaulting to EfiBltVideoFill.
    gop->Blt(gop, &GraphicsColor, EfiBltVideoFill, 0, 0, xPos, yPos, 1, 1, 0);
}

// This creates a filled box of pixels.
// NOTE : Call SetGraphicsColor prior to using this function.
void CreateFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h)
{
    // TODO : Add in a choice instead of defaulting to EfiBltVideoFill.
    gop->Blt(gop, &GraphicsColor, EfiBltVideoFill, 0, 0, xPos, yPos, w, h, 0);
}

typedef struct BLOCKINFO
{
	int                  MagicNumber;
    unsigned long long*  BaseAddress;
    unsigned long long   BufferSize;
    unsigned int         ScreenWidth;
    unsigned int         ScreenHeight;
    unsigned int         PixelsPerScanLine;
} BLOCKINFO;

BLOCKINFO bi;

// This creates a filled box of pixels using the BLOCKINFO.
// NOTE : Call SetGraphicsColor prior to using this function.
void CreateBufferFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h)
{
    // This functions puts a color filled box on the screen
    UINT32 ByteOffset = 4;
    if(xPos < 0){xPos = 0;}
    if(yPos < 0){yPos = 0;}
    if(w < 1){w = 1;}
    if(h < 1){h = 1;}
    UINT32 x;
    UINT32 y      = yPos;
    UINT32 width  = (xPos + w) * ByteOffset;
    UINT32 height = yPos + h;

    for(y = yPos; y <= height; y++)
    {
        for(x = xPos * ByteOffset; x <= width; x+=ByteOffset)
        {
            *(UINT32*)(x + (y * bi.PixelsPerScanLine * ByteOffset) + bi.BaseAddress) = *(UINT32*)&GraphicsColor;
        }
    }
}

void Delay(UINTN d)
{
    // The Stall function is set is microseconds. So you have to convert
    // from microseconds ( µs ) to milliseconds ( ms ). EXAMPLE : 1ms = 1000µs.
    // The microsecond is the smallest unit of time measurement the EFI provides.
    SystemTable->BootServices->Stall(d * 1000);
}

void Delay1()
{
    // The Stall function is set as microseconds. We stall 1 microsecond.
    SystemTable->BootServices->Stall(1);
}

void COLD_REBOOT()
{
    // Hardware Reboot
    SystemTable->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, 0);
}

void WARM_REBOOT()
{
    // Software reboot
    SystemTable->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, 0);
}

void SHUTDOWN()
{
    // Shuts off the computer
    // NOTE : This does not work in VirtualBox.
    // WORKS in QEMU !!!
    SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
}

void InitializeFILESYSTEM()
{
    // To load a file, you must have a file system. EFI takes advantage of the FAT32 file system.
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	EFI_DEVICE_PATH_PROTOCOL *DevicePath;
	
    if((SystemTable->BootServices->HandleProtocol(ImageHandle, &EFI_LOADED_IMAGE_PROTOCOL_GUID, (void**)&LoadedImage)) == EFI_SUCCESS)
	{
		if((SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_DEVICE_PATH_PROTOCOL_GUID, (void**)&DevicePath)) == EFI_SUCCESS)
		{
			if((SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID, (void**)&Volume)) == EFI_SUCCESS)
			{
				if((Volume->OpenVolume(Volume, &RootFS)) != EFI_SUCCESS)
				{
					Print(L"Loading Root File System FAILED!");
				}
			} else {
		        
	        }
		} else {
		    Print(L"DevicePath FAILED!");
	    }
	} else {
		Print(L"LoadedImage FAILED!");
	}
}

EFI_FILE_PROTOCOL* getFile(CHAR16* FileName)
{
    // This opens a file from the EFI FAT32 file system volume.
    // It loads from root, so you must supply full path if the file is not in the root.
    // Example : "somefolder//myfile"  <--- Notice the double forward slash.
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    if((RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0)) == EFI_NOT_FOUND)
	{
		SetColor(EFI_CYAN);
		Print(L"\r\nWARNING : Unable to find File.\r\n\r\n");
	}
    
    return FileHandle;
}

EFI_FILE_PROTOCOL* getDir(CHAR16* DirName)
{
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    if((RootFS->Open(RootFS, &FileHandle, DirName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY)) == EFI_NOT_FOUND)
	{
		SetColor(EFI_CYAN);
		Print(L"\r\nWARNING : Unable to find Directory.\r\n\r\n");
	}
    
    return FileHandle;
}

void closeFile(EFI_FILE_PROTOCOL* FileHandle)
{
    if((FileHandle->Close(FileHandle)) != EFI_SUCCESS)
	{
        SetColor(EFI_BROWN);
        Print(L"Closing File Handle FAILED\r\n");
	}
}

void closeDir(EFI_FILE_PROTOCOL* FileHandle)
{
    if((FileHandle->Close(FileHandle)) != EFI_SUCCESS)
	{
		SetColor(EFI_BROWN);
		Print(L"Closing Directory Handle FAILED\r\n");
	}
}

EFI_FILE_PROTOCOL* createFile(CHAR16* FileName)
{
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    if((RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0)) == EFI_NOT_FOUND)
	{
		SetColor(EFI_CYAN);
		Print(L"\r\nWARNING : Unable to create file. Please check your path.\r\n\r\n");
	}
    
    return FileHandle;
}

UINTN* GetFileSize (EFI_FILE_PROTOCOL* FileName)
{
    UINTN* Size = 0;
    FileName->SetPosition(FileName, 0xFFFFFFFFFFFFFFFFULL);
    FileName->GetPosition(FileName, Size);
	FileName->SetPosition(FileName, 0);
    return Size;
}

void readFile(CHAR16* FileName)
{
	// We get the file size, allocate memory for it,
    // read the file into the buffer, then we close the file.
    EFI_FILE_PROTOCOL* FileHandle = getFile(FileName);
    if(FileHandle != NULL)
    {
		UINT64* FileSize = (UINT64*)GetFileSize(FileHandle);
		if((SystemTable->BootServices->AllocatePool(EfiLoaderData, *FileSize, (void**)&OSBuffer_Handle)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
			Print(L"Allocating Pool FAILED\r\n");
		}

        if((FileHandle->Read(FileHandle, FileSize, OSBuffer_Handle)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
			Print(L"Reading File FAILED\r\n");
		}

        closeFile(FileHandle);
    }
}

void removeDir(CHAR16* dirName)
{
	EFI_FILE_PROTOCOL* FileHandle = getDir(dirName);
	if(FileHandle != NULL)
	{
		if((FileHandle->Delete(FileHandle)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
		    Print(L"Deleting Directory FAILED\r\n");
		}
	}
}

void makeDir(CHAR16* dirName)
{
    EFI_FILE_PROTOCOL* FileHandle = NULL;
	
    if((RootFS->Open(RootFS, &FileHandle, dirName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY)) != EFI_SUCCESS)
	{
		SetColor(EFI_BROWN);
		Print(L"Creating Directory FAILED\r\n");
	}
	closeDir(FileHandle);
}

void deleteFile(CHAR16* FileName)
{
	EFI_FILE_PROTOCOL* FileHandle = getFile(FileName);
	if(FileHandle != NULL)
	{
		if((FileHandle->Delete(FileHandle)) != EFI_SUCCESS)
		SetColor(EFI_BROWN);
		Print(L"Deleting File FAILED\r\n");
	}
}

void WriteToFile(char* buf, CHAR16* FileName)
{
	UINT64 fileSize = strlen(buf);
	EFI_FILE_PROTOCOL* writefilehandle = createFile(FileName);
	if(writefilehandle != NULL)
	{

		if((writefilehandle->Write(writefilehandle, &fileSize, buf)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
			Print(L"Writing to File FAILED\r\n");
		}

		closeFile(writefilehandle);
	}
}

typedef struct PIXELPOSITIONS
{
    UINT32 PixelxPos;
    UINT32 PixelyPos;
} PIXELPOSITIONS;

PIXELPOSITIONS* pixelpos;

// Font / printf()   <--- No longer used in EFI file.
UINT32 FontSpacing;
void SetFontSpacing(UINT32 fspacing)
{
	if(fspacing < 0) { fspacing = 0; }
	FontSpacing = fspacing;
}

UINT32 CurrentFontSize;
void SetFontSize(UINT32 fs)
{
	if(fs < 4) { fs = 4; }
	CurrentFontSize = (UINT32)((fs / 4));
}

void GetCharacter(UINT32 character, UINT32 xPos, UINT32 yPos)
{
	pixelpos->PixelxPos = xPos;
	pixelpos->PixelyPos = yPos;
	UINT32 SelectASCII  = character;

	UINT32 mcX = pixelpos->PixelxPos;
	UINT32 mcY = pixelpos->PixelyPos;
	UINT32 pPos = 0;
	UINT32 sPos = 0;
	UINT32 FontSize = CurrentFontSize;

	for(UINT32 t = (SelectASCII * 128); t < ((SelectASCII * 128) + 128); t++)
	{
		if(pPos > 7)
		{
			pPos = 0;
			mcY += FontSize;
			mcX = pixelpos->PixelxPos;
			sPos++;
			if(sPos > 15)
			{
				sPos = 0;
				mcY += FontSize;
			}
		}
		UINT32 ASCIIColor = 0; // asciifont[t];    <--- No longer used in EFI file
		switch(ASCIIColor)
		{
			case 0:
			{
				break;
			}
			case 1:
			{
				CreateFilledBox(mcX, mcY, FontSize, FontSize);
				break;
			}
		}
		mcX += FontSize;
		pPos++;
	}
}

typedef struct CPOS
{
	UINT32 cmX;
	UINT32 cmY;
} CPOS;

CPOS* cursorPos;

void SetCursorPosition(UINT32 x, UINT32 y)
{
	cursorPos->cmX = x;
	cursorPos->cmY = y;
	if(cursorPos->cmX < 0){cursorPos->cmX = 0;}
	if(cursorPos->cmY < 0){cursorPos->cmY = 0;}
}

void printf(void* str) // Our Custom printf function.
{
	for (char *i = str; *i; i++)
	{
		if(*i == 10)
		{
			cursorPos->cmY += 10 + ((CurrentFontSize * 5) + FontSpacing);
		} else if(*i == 13)
		{
			cursorPos->cmX = 0;
		} else {
			GetCharacter(*i, cursorPos->cmX, cursorPos->cmY);
		    cursorPos->cmX += ((CurrentFontSize * 7) + FontSpacing);
		}
    }
}

void InitializeGOP()
{
    // We initialize the Graphics Output Protocol.
    // This is used instead of the VGA interface.
    SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 0, (void**)&gop);
	
    bi.BaseAddress        = (void*)gop->Mode->FrameBufferBase;
    bi.BufferSize         = gop->Mode->FrameBufferSize;
    bi.ScreenWidth        = gop->Mode->Info->HorizontalResolution;
    bi.ScreenHeight       = gop->Mode->Info->VerticalResolution;
    bi.PixelsPerScanLine  = gop->Mode->Info->PixelsPerScanLine;
}

void InitializeSystem()
{
    ResetScreen();
    InitializeGOP();
	InitializeFILESYSTEM();
}

#endif // EFILIBS_H