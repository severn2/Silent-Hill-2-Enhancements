#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Patches.h"
#include "Common\Utils.h"
#include "Logging\Logging.h"

void* jmp_return;
void* jmp_to_loop;

DWORD* muteSound;
DWORD EventIndex;
BYTE MenuEventIndex;

__declspec(naked) void __stdcall FixInventoryBGMBugASM()
{
	EventIndex = GetEventIndex();
	MenuEventIndex = GetMenuEvent();

	if (MenuEventIndex == 0xd || MenuEventIndex == 0x11 && EventIndex == 0x0)
	{
		if (EventIndex == 0xb)
		{
			*muteSound = 0xF;
		}

		if (EventIndex > 3 && EventIndex < 10 || EventIndex == 0x10 || MenuEventIndex == 0x11)
		{
			__asm
			{
				jmp jmp_return
			}
		}
	}
	__asm
	{
		jmp jmp_to_loop
	}
}

void PatchInventoryBGMBug()
{
	
	constexpr BYTE BuggyBGMBytes[] = { 0x83, 0xf8, 0x04, 0x75, 0x0d, 0x68 };
	DWORD BuggyBGMAddr = SearchAndGetSpecifiedAddr(0x05166c9, 0x5169F9, 0x516319, BuggyBGMBytes, sizeof(BuggyBGMBytes), 0x1f);

	if (!BuggyBGMAddr)
	{
		Logging::Log() << __FUNCTION__ << " Error: failed to find memory address!";
		return;
	}
	memcpy(&muteSound, (DWORD*)(BuggyBGMAddr - 4), sizeof(DWORD));
	jmp_return = reinterpret_cast<void*>(BuggyBGMAddr + 0x24);
	jmp_to_loop = reinterpret_cast<void*>(BuggyBGMAddr + 0x31);

	WriteJMPtoMemory(reinterpret_cast<BYTE*>(BuggyBGMAddr), *FixInventoryBGMBugASM,0x24);
}