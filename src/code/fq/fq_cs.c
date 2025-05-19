#include "fq/cs.h"
#include "fq/fq.h"

#include "array_count.h"
#include "printf.h"
#include "segmented_address.h"
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
    return gFQ.skipIntro;
}

void FqCs_IntroMod(PlayState* play, Player* player) {
    play->viewpoint = VIEWPOINT_LOCKED;
    Camera_RequestBgCam(GET_ACTIVE_CAM(play), play->viewpoint - 1);
    FqCs_SetPlayerPosYaw(player, 0, 0, 60, 0x8000);
}

void FqCs_SariaIntroMod(PlayState* play, Player* player) {
    FqCs_SetPlayerPosYaw(player, -30.0f, 100.0f, 1025.0f, 0x8000);
}

// ================= TABLE

static CsHandlerEntry gFQCsHandlers[] = {
    { ENTR_LINKS_HOUSE_0, 0xFFF1, NULL, FqCs_IntroCheck, FqCs_IntroMod },
    { 0, 0, gKokiriForestSariaGreetingCs, NULL, FqCs_SariaIntroMod },
};

static CsHandlerEntry* sQueuedEntry = NULL;

/**
 * This function runs early in the Play_Init process to override the scene layer if necessary.
 * It also sets cutsceneIndex to zero to skip the cutscene.
 *
 * This only applies to scene layer cutscenes that start right away on scene load.
 */
s32 FqCs_PreSceneOverride(PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gFQCsHandlers); i++) {
        CsHandlerEntry* entry = &gFQCsHandlers[i];

        if (gSaveContext.save.entranceIndex == entry->entranceIndex &&
            gSaveContext.save.cutsceneIndex == entry->cutsceneIndex && (entry->check == NULL || entry->check(play))) {
            gSaveContext.save.cutsceneIndex = 0;
            sQueuedEntry = entry;

            return true;
        }
    }

    return false;
}

/**
 * Runs every frame to either:
 * - Process a queued entry that got detected during Play_Init
 * - Catch a cutscene that started mid-gameplay
 *
 * This gets called just before `Cutscene_UpdateScripted` to prevent a
 * cutscene from playing if necessary.
 */
void FqCs_Update(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;

    if (sQueuedEntry != NULL) {
        // If there was a match in the PreSceneOverride check, it was saved in `sQueuedEntry`.
        // This prevents having to search through the table again to find it.
        sQueuedEntry->modifier(play, player);
        sQueuedEntry = NULL;
    } else if (gSaveContext.cutsceneTrigger != 0) {
        // Only search the table if a "non scene layer" cutscene is playing.
        for (i = 0; i < ARRAY_COUNT(gFQCsHandlers); i++) {
            CsHandlerEntry* entry = &gFQCsHandlers[i];

            if (play->csCtx.script == SEGMENTED_TO_VIRTUAL(entry->cs) && (entry->check == NULL || entry->check(play))) {
                gSaveContext.save.cutsceneIndex = 0;
                gSaveContext.cutsceneTrigger = 0;
                entry->modifier(play, player);
            }
        }
    }
}
