#include <Windows.h>
#include <libloaderapi.h>

#include "EntityFactory.h"
#include "../Entities/Player.h"
#include "../Memory/Mem.h"
#include "../Memory/Pattern.h"

EntityFactory::EntityFactory()
{
	m_hProc = reinterpret_cast<uintptr_t>(GetModuleHandle(EXECUTABLE_NAME));
}

Player* EntityFactory::GetLocalPlayer()
{
	if (!m_pLocalPlayer)
	{
		std::vector<uint32_t> m_vPlayerOffsets = { 0x0, 0x118 };
		m_pLocalPlayer = *memory::FindDMAAddy<Player*>(m_hProc + 0x312930, m_vPlayerOffsets);
	}
	return m_pLocalPlayer;
}

void** EntityFactory::GetClientList()
{
	if (!m_pClientList)
	{
		// When dereferenced, pointer will be 64 bit
		char* match = pattern::FindPattern("\x48\x8B?????\x4C\x8B??\x41\x83\x38", "sauerbraten.exe");
		if (!match)
		{
			return nullptr;
		}
		// Cast to 32-bit address, this will give us the address relative to RIP
		uint32_t relOffset = *reinterpret_cast<uint32_t*>(match + 0x3);
		// [RIP] + 7 + relativeAddress
		m_pClientList = reinterpret_cast<void**>(match + 0x7 + relOffset);
	}
	return m_pClientList;
}

void** EntityFactory::GetBotList()
{
	if (!m_pBotList)
	{
		char* match = pattern::FindPattern("\x48\x8B?????\x48\x8B??\x48\x85?\x0F\x84????\x83\xB9\xA0\x03\x00\x00", "sauerbraten.exe");
		if (!match)
		{
			return nullptr;
		}

		uint32_t relOffset = *reinterpret_cast<uint32_t*>(match + 0x3);
		m_pBotList = reinterpret_cast<void**>(match + 0x7 + relOffset);
	}

	return m_pBotList;
}

Player* EntityFactory::GetPlayer(size_t index)
{
	EntityListItem* item = reinterpret_cast<EntityListItem*>(**reinterpret_cast<uintptr_t**>(GetClientList()) + index * 0x8);
	
	return item ? reinterpret_cast<Player*>(item->m_pPlayer) : nullptr;
}

Player* EntityFactory::GetBot(size_t index)
{
	return reinterpret_cast<Player*>(**reinterpret_cast<uintptr_t**>(GetBotList()) + index * 0x8);
}

uint32_t EntityFactory::NumOfClients()
{
	static uint32_t* pNumClients = nullptr;
	if (!pNumClients)
	{
		pNumClients = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(GetClientList()) + 0xC);
	}

	return *pNumClients;
}

uint32_t EntityFactory::NumOfBots()
{
	static uint32_t* pNumBots = nullptr;
	if (!pNumBots)
	{
		pNumBots = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(GetBotList()) + 0xC);
	}

	return *pNumBots;
}
