#pragma once

#include <vector>
#include <memory>

struct HookState
{
public:
	// Pointer to the function to be hooked
	uint8_t* m_pSource = nullptr;
	// Pointer to the memory where we want to redirect the code execution to
	uint8_t* m_pDestination = nullptr;
	// Function Pointer to the gateway returned by the Trampoline Hook
	void* m_pGateway = nullptr;
	// The bytes that are overriden by our detour jump
	std::vector<uint8_t> m_vOriginalBytes;
};

class HookFactory
{
private:
	std::vector<std::unique_ptr<HookState>> m_vHooks;

	bool Hook(uint8_t* src, uint8_t* dst, size_t len);
	uint8_t* TrampHook(uint8_t* src, uint8_t* dst, size_t len);
	// Save the original bytes so that we can restore them when unhooking
	void BackupBytes(std::unique_ptr<HookState>& state, size_t length);

public:
	~HookFactory()
	{
		Unhook();
	}
	// Reverts all hooks if any are placed and resets the Factory
	void Unhook();

	/**
	* Place a Trampoline Hook at the beginning of a function and backs up any overwritten bytes
	* @param src: A pointer to the beginning of the function to be hooked
	* @param dst: A pointer to the function where the code execution should be detoured to
	* @param length: The number of bytes that need to overwritten by the detour jump
	* @returns A Function Pointer to the gateway allocated by the Trampoline Hook, nullptr if anything fails
	*/
	template<class Fn> Fn HookFunction(uint8_t* src, uint8_t* dst, size_t length)
	{
		std::unique_ptr<HookState> pHookState = std::make_unique<HookState>();
		pHookState->m_pSource = src;
		pHookState->m_pDestination = dst;
		BackupBytes(pHookState, length);
		pHookState->m_pGateway = TrampHook(src, dst, length);
		
		if (pHookState->m_pGateway)
		{
			m_vHooks.push_back(pHookState);
			return reinterpret_cast<Fn>(pHookState->m_pGateway);
		}
		else
		{
			return nullptr;
		}
	}
};
