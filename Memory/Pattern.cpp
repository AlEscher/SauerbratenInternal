#include "Pattern.h"

#include "../Utility/MicrosoftShit.h"
#include <TlHelp32.h>


char* TO_CHAR(wchar_t* string);
PEB* GetPEB();
LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name);

// Start from begin and scan for a pattern, wildcard characters '?' are skipped. Returns nullptr if no match is found
char* ScanForPattern(const char* pattern, char* begin, size_t size)
{
	if (!pattern || !begin)
		return nullptr;

	size_t patternLen = strlen(pattern);

	// Walk through the specified memory region and scan each byte for our pattern
	for (int modIndex = 0; modIndex < size; modIndex++)
	{
		bool match = true;
		// Look if our pattern starts at the current location
		for (int pIndex = 0; pIndex < patternLen; pIndex++)
		{
			// If our pattern doesn't match this part of the memory region anymore, stop comparing and go to the next byte in the module
			if (pattern[pIndex] != '?' && pattern[pIndex] != *reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(begin) + modIndex + pIndex))
			{
				match = false;
				break;
			}
		}

		// If we walked the whole distance of our pattern and never had a mismatch, we found our pattern
		if (match)
		{
			return (begin + modIndex);
		}
	}

	// We didn't find a match for our pattern
	return nullptr;
}

// From guidedhacking.com
// Walk through specified memory and skip bad regions
char* ScanInternal(const char* pattern, char* begin, intptr_t size)
{
	char* match{ nullptr };
	MEMORY_BASIC_INFORMATION mbi{};

	for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
	{
		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

		match = ScanForPattern(pattern, curr, mbi.RegionSize);

		if (match != nullptr)
		{
			break;
		}
	}
	return match;
}

char* pattern::FindPattern(const char* pattern, const char* moduleName)
{
	LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(moduleName);
	if (!ldr)
		return nullptr;

	if (!pattern)
	{
		return nullptr;
	}

	char* match = ScanInternal(pattern, reinterpret_cast<char*>(ldr->DllBase), ldr->SizeOfImage);

	return match;
}






/**
+------------------------------------+
| Stuff taken from guidedhacking.com |
+------------------------------------+
*/

PEB* GetPEB()
{
#ifdef _WIN64
	PEB* peb = (PEB*)__readgsqword(0x60);

#else
	PEB* peb = (PEB*)__readfsdword(0x30);
#endif

	return peb;
}

char* TO_CHAR(wchar_t* string)
{
	size_t len = wcslen(string) + 1;
	char* c_string = new char[len];
	size_t numCharsRead;
	wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
	return c_string;
}

LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name)
{
	LDR_DATA_TABLE_ENTRY* ldr = nullptr;

	PEB* peb = GetPEB();

	LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList;

	LIST_ENTRY curr = head;

	while (curr.Flink != head.Blink)
	{
		LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (mod->FullDllName.Buffer)
		{
			char* cName = TO_CHAR(mod->BaseDllName.Buffer);

			if (_stricmp(cName, name.c_str()) == 0)
			{
				ldr = mod;
				delete[] cName;
				break;
			}
			delete[] cName;
		}
		curr = *curr.Flink;
	}
	return ldr;
}

// Taken from GuidedHacking.com
uintptr_t pattern::GetModuleBaseAddress(uint32_t procId, const char* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				/*wchar_t wszModule[256];
				MultiByteToWideChar(CP_UTF8, 0, modEntry.szModule, -1, wszModule, 16);*/
				char* modName = TO_CHAR(modEntry.szModule);
				if (!_stricmp(modName, modName))
				{
					modBaseAddr = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
					delete[] modName;
					break;
				}
				delete[] modName;
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

HANDLE pattern::GetProcessHandle(const char* procName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	HANDLE hProcess = nullptr;

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			char* exeName = TO_CHAR(entry.szExeFile);
			if (_stricmp(exeName, procName) == 0)
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			}
			delete[] exeName;
		}
	}

	CloseHandle(snapshot);
	return hProcess;
}
