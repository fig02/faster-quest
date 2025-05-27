#include "ultra64.h"
#include "array_count.h"
#include "controller.h"
#include "seqcmd.h"
#include "printf.h"
#include "z64play.h"
#include "z64player.h"
#include "z64save.h"
#include "z64interface.h"
#include "z64scene.h"
#include "z64math.h"
#include "z64game.h"
#include "z64room.h"

s32 sRequestedRoomNumber = -1;

void DebugFq_ProcessRoomChangeRequest(PlayState* play) {
    if (sRequestedRoomNumber >= 0 && play->roomCtx.status == 0) {
        Room_RequestNewRoom(play, &play->roomCtx, sRequestedRoomNumber);
        sRequestedRoomNumber = -1;
    }
}

typedef struct DebugSpawn {
    s16 sceneId;
    u8 room;
    Vec3f pos;
    s16 yaw;
} DebugSpawn;

// This is called in Player_Init
void DebugFq_SetSpawnPos(Player* player, PlayState* play) {
    // DebugSpawn entry = { SCENE_KOKIRI_FOREST, 1, { 2352.0f, -1.0f, -464.0f }, 0x51D5 };
    DebugSpawn entry = { -1, 1, { 2352.0f, -1.0f, -464.0f }, 0x51D5 };

    if (entry.sceneId >= 0 && entry.sceneId == play->sceneId) {
        player->actor.world.pos = entry.pos;
        player->actor.home.pos = player->actor.world.pos;
        player->actor.world.rot.y = player->actor.shape.rot.y = entry.yaw;

        sRequestedRoomNumber = entry.room;
    }
}

// s32 sDebugEntrance = -1;
s32 sDebugEntrance = -1;

s32 DebugFq_LoadToEntrance(GameState* gamestate) {
    if (sDebugEntrance >= 0) {
        Sram_InitDebugSave();

        gSaveContext.save.linkAge = LINK_AGE_CHILD;

        gSaveContext.save.entranceIndex = sDebugEntrance;
        gSaveContext.save.info.playerData.isMagicAcquired = true;
        gSaveContext.save.info.playerData.isDoubleMagicAcquired = true;
        gSaveContext.magicFillTarget = gSaveContext.save.info.playerData.magic;
        gSaveContext.save.info.playerData.magic = 0;
        gSaveContext.magicCapacity = 0;
        gSaveContext.save.info.playerData.magicLevel = gSaveContext.save.info.playerData.magic;

        gSaveContext.gameMode = GAMEMODE_NORMAL;

        SEQCMD_RESET_AUDIO_HEAP(0, 10); // TODO why is there no sound????

        SET_NEXT_GAMESTATE(gamestate, Play_Init, PlayState);

        return true;
    }

    return false;
}

void DebugFq_PrintLinkInfo(PlayState* play) {
    if (CHECK_BTN_ALL(play->state.input->press.button, BTN_L)) {
        Player* player = GET_PLAYER(play);
        PosRot* world = &player->actor.world;

        PRINTF("%f %f %f %4X room:%d\n", world->pos.x, world->pos.y, world->pos.z, world->rot.y,
               play->roomCtx.curRoom.num);
    }
}

void DebugFq_Update(PlayState* play) {
    DebugFq_PrintLinkInfo(play);
    DebugFq_ProcessRoomChangeRequest(play);
}
