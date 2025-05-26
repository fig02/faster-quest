#ifndef FQ_DEBUG_H
#define FQ_DEBUG_H

#include "ultra64.h"
#include "z64play.h"
#include "z64game.h"
#include "z64player.h"

void DebugFq_Update(PlayState* play);
void DebugFq_SetSpawnPos(Player* player, PlayState* play);
s32 DebugFq_LoadToEntrance(GameState* gamestate);

#endif
