#include "Mem.h"

// dst for the Bytes to be overwritten, src pointer to a BYTE array
void memory::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldProtect;
	// Changes the page access rights for the instructions we want to patch, since Code is executable but not writable
	// If you only use WRITE permission, it can create problems if those instructions are being executed
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	// patch the bytes
	memcpy(dst, src, size);
	// restore old memory access permissions
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

void memory::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	// patch the bytes
	memset(dst, 0x90, size);
	// restore old memory access permissions
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}