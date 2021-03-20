#pragma once
#include <windows.h>
#include <vector>

namespace memory
{
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	template <class T> T* FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
	{
		uintptr_t addr = ptr;
		for (unsigned int i = 0; i < offsets.size(); i++)
		{
			// read what's in addr and store it in addr (dereference the pointer)
			addr = *(uintptr_t*)addr;
			// add offset
			addr += offsets[i];
		}
		return reinterpret_cast<T*>(addr);
	}
}

