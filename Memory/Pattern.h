#pragma once
#include <string>

typedef void* HANDLE;

namespace pattern
{
	// Walks the given module and searches for the first occurrence of pattern. If no pattern is found, a nullptr is returned
	char* FindPattern(const char* pattern, const char* moduleName);
	template <typename T>
	/*
		Safely create a pointer from a signature found by pattern scan
	*/
	T* PtrFromSignature(char* sig, int index, bool doubleDeref)
	{
		if (sig)
		{
			if (doubleDeref)
			{
				return **reinterpret_cast<T***>(sig + index);
			}
			else
			{
				return *reinterpret_cast<T**>(sig + index);
			}
		}

		return nullptr;
	}
	uintptr_t GetModuleBaseAddress(uint32_t procId, const char* modName);
	HANDLE GetProcessHandle(const char* procName);
}
