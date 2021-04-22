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
	void** m_pClientList = nullptr;
	void** m_pBotList = nullptr;
	uintptr_t m_hProc = 0U;

public:
	EntityFactory();

	Player* GetLocalPlayer();
	Player* GetPlayer(size_t index);
	Player* GetBot(size_t index);
	void** GetClientList();
	void** GetBotList();
	uint32_t NumOfClients();
	uint32_t NumOfBots();
	inline void ClearCache()
	{
		m_pLocalPlayer = nullptr;
		m_pClientList = nullptr;
		m_pBotList = nullptr;
	}
};