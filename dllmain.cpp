#include <cstdio>
#include <iostream>

#include "framework.h"
#include "Utility/DebugConsole.h"

#include "Entities/Player.h"
#include "Factory/EntityFactory.h"

// Uncomment this to allocate a console in release mode
// #define DEBUG_RELEASE_MODE

#if defined(_DEBUG) || defined(DEBUG_RELEASE_MODE)
FILE* f;
#endif

EntityFactory* entityFactory = nullptr;

void PrintDebug(const char* message)
{
#if defined(_DEBUG) || defined(DEBUG_RELEASE_MODE)
	std::cout << static_cast<float>(clock()) / CLOCKS_PER_SEC << ": " << message << std::endl;
#endif
}

bool Initialize()
{
	entityFactory = new EntityFactory;
	if (!entityFactory)
		return false;

	return true;
}

void Uninject(PVOID base)
{
	PRINT_DEBUG("Uninjecting!");
	Sleep(500);

	delete entityFactory;

	// Free our console if we are in Debug mode
#if defined(_DEBUG) || defined(DEBUG_RELEASE_MODE)
	fclose(f);
	FreeConsole();
#endif

	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 0);
}

DWORD WINAPI OnDllAttach(PVOID base)
{
	PRINT_DEBUG("OnDllAttach called");

	// Wait for the last dll to be loaded
	while (!GetModuleHandle(L"libjpeg-9.dll"))
	{
		Sleep(100);
	}

	PRINT_DEBUG("Initializing");
	if (!Initialize())
	{
		Uninject(base);
	}

	while (!GetAsyncKeyState(VK_END))
	{
		void* entityList = entityFactory->GetClientList();
		void* botList = entityFactory->GetBotList();
		std::cout << std::hex << botList << std::endl;

		for (size_t i = 0; i < entityFactory->NumOfClients(); i++)
		{
			Player* player = entityFactory->GetPlayer(i);
			if (player)
			{
				std::cout << std::dec << player->health << std::endl;
			}
			else
			{
				// We reached the end of the list
				break;
			}
		}
		for (size_t i = 0; i < entityFactory->NumOfBots(); i++)
		{
			Player* player = entityFactory->GetBot(i);
			if (player)
			{
				std::cout << std::dec << player->health << std::endl;
			}
			else
			{
				// We reached the end of the list
				break;
			}
		}

		Sleep(500);
	}

	Uninject(base);

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
#if defined(_DEBUG) || defined(DEBUG_RELEASE_MODE)
		AllocConsole();
		freopen_s(&f, "CONOUT$", "w", stdout);
#endif

		PRINT_DEBUG("Injecting");

		// This fails when injecting Release mode through Manual Map, Thread Hijack etc...
		if (!DisableThreadLibraryCalls(hModule))
		{
#if defined(_DEBUG) || defined(DEBUG_RELEASE_MODE)
			std::cout << "DisableThreadLibraryCalls failed. hModule: 0x" << std::hex << hModule << std::dec << std::endl;
#endif
		}

		PRINT_DEBUG("Creating Thread");

		HANDLE t = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OnDllAttach, hModule, NULL, NULL);

		PRINT_DEBUG("Thread created");
		// Prevent a Handle leak
		if (t)
			CloseHandle(t);
	}

	return TRUE;
}

