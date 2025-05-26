#include "ultra64.h"
#include "array_count.h"
#include "controller.h"
#include "printf.h"
#include "z64play.h"
#include "z64player.h"
#include "z64math.h"
#include "z64room.h"

typedef struct DebugSpawn {
    u8 sceneId;
    u8 room;
    Vec3f pos;
    s16 yaw;
} DebugSpawn;

DebugSpawn sSpawnTable[] = {
    { SCENE_KOKIRI_FOREST, 1, { 2352.0f, -1.0f, -464.0f }, 0x51D5},
};

s32 sRequestedRoomNumber = -1;

void DebugFq_ProcessRoomChangeRequest(PlayState* play) {
    if (sRequestedRoomNumber >= 0 && play->roomCtx.status == 0) {
        Room_RequestNewRoom(play, &play->roomCtx, sRequestedRoomNumber);
        sRequestedRoomNumber = -1;
    }
}

// This is called in Player_Init
void DebugFq_SetSpawnPos(Player* player, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sSpawnTable); i++) {
        DebugSpawn* entry = &sSpawnTable[i];
        
        if (entry->sceneId == play->sceneId) {
            player->actor.world.pos = entry->pos;
            player->actor.home.pos = player->actor.world.pos;
            player->actor.world.rot.y = player->actor.shape.rot.y = entry->yaw;

            sRequestedRoomNumber = entry->room;
        }
    }
}

void DebugFq_PrintLinkInfo(PlayState* play) {
    if (CHECK_BTN_ALL(play->state.input->press.button, BTN_L)) {
        Player* player = GET_PLAYER(play);
        PosRot* world = &player->actor.world;

        PRINTF("%f %f %f %4X room:%d\n", world->pos.x, world->pos.y, world->pos.z, world->rot.y, play->roomCtx.curRoom.num);
    }
}

void DebugFq_Update(PlayState* play) {
    DebugFq_PrintLinkInfo(play);
    DebugFq_ProcessRoomChangeRequest(play);
}
