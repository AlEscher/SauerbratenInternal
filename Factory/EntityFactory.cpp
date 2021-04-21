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

void* EntityFactory::GetEntityList()
{
	if (!m_pEntityList)
	{
		// When dereferenced, pointer will be 64 bit
		char* match = pattern::FindPattern("\x48\x8B?????\x4C\x8B??\x41\x83\x38", "sauerbraten.exe");
		// Cast to 32-bit address, this will give us the address relative to RIP
		uint32_t relOffset = *reinterpret_cast<uint32_t*>(match + 0x3);
		// [RIP] + 7 + relativeAddress
		m_pEntityList = *reinterpret_cast<void**>(match + 0x7 + relOffset);
	}
	return m_pEntityList;
}

Player* EntityFactory::GetPlayer(size_t index)
{
	EntityListItem* item = reinterpret_cast<EntityListItem*>(*reinterpret_cast<uintptr_t*>(GetEntityList()) + index * 0x8);
	
	return item ? reinterpret_cast<Player*>(item->m_pPlayer) : nullptr;
}
