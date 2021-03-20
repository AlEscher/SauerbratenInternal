#pragma once

#include <cstdint>

#include "Entity.h"
#include "Structs.h"

class Player : public Entity
{
public:
	char pad_0000[832]; //0x0000
	int health; //0x0340
	char pad_0344[4]; //0x0344
	int armour; //0x0348
	char pad_034C[8]; //0x034C
	uint32_t currWeaponIdx; //0x0354
	char pad_0358[8]; //0x0358
	uint32_t shotGunAmmo; //0x0360
	uint32_t heavyGunAmmo; //0x0364
	uint32_t rpgAmmo; //0x0368
	uint32_t rifleAmmo; //0x036C
	uint32_t grenadeAmmo; //0x0370
	uint32_t pistolAmmo; //0x0374
	char pad_0378[28]; //0x0378
	Vector3 coordinates; //0x0394
	char pad_03A0[100]; //0x03A0
	int32_t frags; //0x0404
	char pad_0408[128]; //0x0408
	char mapName[8]; //0x0488
	char pad_0490[1016]; //0x0490
}; //Size: 0x0888
//static_assert(sizeof(Player) == 0x888);