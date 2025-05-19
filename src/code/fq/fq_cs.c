#include "fq/cs.h"
#include "fq/fq.h"

#include "array_count.h"
#include "z64camera.h"
#include "z64play.h"
#include "z64player.h"
#include "z64save.h"

#include "assets/scenes/overworld/spot04/spot04_scene.h"

// ================= HELPERS

void FqCs_SetPlayerPosYaw(Player* player, f32 x, f32 y, f32 z, s16 yaw) {
    player->actor.world.pos.x = player->actor.home.pos.x = x;
    player->actor.world.pos.y = player->actor.home.pos.x = y;
    player->actor.world.pos.z = player->actor.home.pos.x = z;

    player->actor.world.rot.y = player->actor.shape.rot.y = yaw;
}

// ================= CALLBACKS

s32 FqCs_IntroCheck(PlayState* play) {
    return gFQ.skip_intro;
}

void FqCs_IntroMod(PlayState* play, Player* player) {
    play->viewpoint = VIEWPOINT_LOCKED;
    Camera_RequestBgCam(GET_ACTIVE_CAM(play), play->viewpoint - 1);
    FqCs_SetPlayerPosYaw(player, 0, 0, 60, 0x8000);
}

void FqCs_SariaMod(PlayState* play, Player* player) {
    
}

// ================= TABLES

#define SEARCH_MODE_PRE 0
#define SEARCH_MODE_POST 1

#define ENTRANCE_NONE -1
#define CS_INDEX_NONE 0xFFFD

static CsHandlerEntry gFQCsHandlers[] = {
    { ENTR_LINKS_HOUSE_0, 0xFFF1, NULL, FqCs_IntroCheck, FqCs_IntroMod },
    { ENTRANCE_NONE, CS_INDEX_NONE, gKokiriForestSariaGreetingCs, NULL, FqCs_SariaMod },
};

CsHandlerEntry* FqCs_FindEntry(s32 mode) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gFQCsHandlers); i++) {
        CsHandlerEntry* entry = &gFQCsHandlers[i];

        switch (mode) {
            case SEARCH_MODE_PRE:
                break;

            case SEARCH_MODE_POST:
                break;
        }
    }
}

/**
 * This function runs early in the Play_Init process to override the scene layer if necessary.
 * It also sets cutsceneIndex to zero to skip the cutscene.
 * 
 * This only applies to scene layer cutscenes that start right away on scene load.
 */
void FqCs_PreSceneOverride(PlayState* play) {
    CsHandlerEntry* entry = FqCs_FindEntry();

    if (entry != NULL) {
        if (gSaveContext.save.entranceIndex == entry->entranceIndex && 
            gSaveContext.save.cutsceneIndex == entry->cutsceneIndex &&
            (entry->check == NULL || entry->check(play))) {
            gSaveContext.save.cutsceneIndex = 0;
        }
    }
}

void FqCs_PostSceneOverride(PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gFQCsHandlers); i++) {
        CsHandlerEntry* entry = &gFQCsHandlers[i];

        if (entry->cs != NULL && entry->cs == play->csCtx.script && 
            (entry->check == NULL || entry->check(play))) {
            gSaveContext.save.cutsceneIndex = 0;
            gSaveContext.cutsceneTrigger = 0;
        }
    }
}

/**
 * Applies necessary modifications after a cutscene has been skipped.
 */
void FqCs_PostCsModifier(PlayState* play, Player* player) {
    if (entry != NULL && entry->modifier != NULL) {
        entry->modifier(play, player);
    }
    
}
