#include <wtypes.h>
#include <winnt.h>
#include <memoryapi.h>

#include "Hook.h"


void HookFactory::Unhook()
{
	for (std::unique_ptr<HookState>& state : m_vHooks)
	{
		// Restore original bytes
		std::copy(state->m_vOriginalBytes.begin(), state->m_vOriginalBytes.end(), state->m_pSource);
		// Free gateway
		VirtualFree(state->m_pGateway, 0, MEM_RELEASE);
	}

	m_vHooks.clear();
}

void HookFactory::BackupBytes(std::unique_ptr<HookState>& state, size_t length)
{
	if (state->m_pDestination && state->m_pSource)
	{
		state->m_vOriginalBytes.resize(length);
		std::copy(state->m_pSource, state->m_pSource + length, state->m_vOriginalBytes.begin());
	}
}

bool HookFactory::Hook(uint8_t* src, uint8_t* dst, size_t len)
{
	if (len < 5) return false;

	DWORD oldProtect;
	if (!VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProtect))
		return false;

	memset(src, 0x90, len);
	uintptr_t relAddy = (uintptr_t)(dst - src - 5);
	*src = (char)0xE9;
	*(uintptr_t*)(src + 1) = (uintptr_t)relAddy;

	// Reset old access permissions
	if (!VirtualProtect(src, len, oldProtect, &oldProtect))
		return false;

	return true;
}

/*
	Wrapper for Hook which also allocates a gateway (a memory region where the bytes that we overwrite
	with our jump are executed). At the end of the gateway a jump to the original function is placed.
	Returns the gateway
*/
uint8_t* HookFactory::TrampHook(uint8_t* src, uint8_t* dst, size_t len)
{
	if (len < 5)
	{
		return 0;
	}

	uint8_t* gateway = nullptr;
	gateway = reinterpret_cast<uint8_t*>(VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	if (!gateway)
		return nullptr;

	// copy stolen bytes into our gateway
	memcpy(gateway, src, len);

	uintptr_t jumpAddy = (uintptr_t)(src - gateway - 5);
	// Write unconditional jump
	*(gateway + len) = (char)0xE9;
	// Write address to our gateway
	*(uintptr_t*)(gateway + len + 1) = jumpAddy;

	if (Hook(src, dst, len))
	{
		return gateway;
	}
	else
	{
		return  nullptr;
	}
}