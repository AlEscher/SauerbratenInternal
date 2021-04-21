#pragma once

#include "../Entities/Entity.h"
#include "../Globals.h"

#include <vector>

class Player;

struct EntityListItem
{
	char pad[0x118];
	Player* m_pPlayer;
};

class EntityFactory
{
private:
	Player* m_pLocalPlayer = nullptr;
	void* m_pEntityList = nullptr;
	uintptr_t m_hProc = 0U;

public:
	EntityFactory();

	Player* GetLocalPlayer();
	Player* GetPlayer(size_t index);
	void* GetEntityList();
	inline void ClearCache()
	{
		m_pLocalPlayer = nullptr;
		m_pEntityList = nullptr;
	}
};