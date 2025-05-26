#include "fq/cs.h"
#include "fq/fq.h"

#include "array_count.h"
#include "printf.h"
#include "sfx.h"
#include "segmented_address.h"
#include "z64actor.h"
#include "z64camera.h"
#include "z64play.h"
#include "z64player.h"
#include "z64save.h"
#include "z_actor_dlftbls.h"

// ================= HELPERS

void FqCs_SkipTitleCard(PlayState* play) {
    play->actorCtx.titleCtx.alpha = 0;
    play->actorCtx.titleCtx.durationTimer = 0;
    play->actorCtx.titleCtx.delayTimer = 0;
}

void FqCs_SetPlayerPosYaw(Player* player, f32 x, f32 y, f32 z, s16 yaw, u8 setNavi) {
    player->actor.world.pos.x = player->actor.home.pos.x = x;
    player->actor.world.pos.y = player->actor.home.pos.x = y;
    player->actor.world.pos.z = player->actor.home.pos.x = z;

    player->actor.world.rot.y = player->actor.shape.rot.y = yaw;

    if (setNavi) {
        Actor* navi = player->naviActor;

        navi->world = player->actor.world;
        navi->shape.rot.y = navi->world.rot.y;
    }
}

// ================= SKIP CALLBACKS

void FqCs_IntroSkip(PlayState* play, Player* player) {
    play->viewpoint = VIEWPOINT_LOCKED;
    Camera_RequestBgCam(GET_ACTIVE_CAM(play), play->viewpoint - 1);
    FqCs_SetPlayerPosYaw(player, 0, 0, 60, 0x8000, true);
}

// ================= TABLE

// scene cutscenes
#include "assets/scenes/overworld/spot04/spot04_scene.h"
#include "assets/scenes/indoors/link_home/link_home_scene.h"
#include "assets/scenes/dungeons/ydan/ydan_scene.h"

// alternate cutscenes
#include "assets/scenes/kokiri_forest/saria_greeting.h"
#include "assets/scenes/kokiri_forest/dekutree_intro.h"

// actor cutscenes
extern CutsceneData gDekuTreeMeetingCs[];

#define CMP_NONE -1
#define ALT_CS_NONE NULL

// clang-format off
static CsHandlerEntry gFQCsHandlers[] = {
    { SCENE_LINKS_HOUSE, 0xFFF1, gLinkHouseIntroSleepCs, ALT_CS_NONE, FqCs_IntroSkip },
    { SCENE_KOKIRI_FOREST, CMP_NONE, gKokiriForestSariaGreetingCs, gAltKokiriForestSariaGreetingCs, NULL },
    { SCENE_KOKIRI_FOREST, ACTOR_BG_TREEMOUTH, gDekuTreeMeetingCs, gAltDekuTreeMeetingCs, NULL },
    { SCENE_DEKU_TREE, CMP_NONE, gDekuTreeIntroCs, NULL, NULL },
};
// clang-format on

static CsHandlerEntry* sQueuedEntry = NULL;

s32 FqCs_CheckSpecialCases(CsHandlerEntry* entry) {
    if (entry->sceneId == SCENE_LINKS_HOUSE && entry->cmp == 0xFFF1) {
        // Check FQ settings for intro skip
        return gFQ.cfg.skipIntro;
    }

    // Default to skipping cutscene if there are no special cases
    return true;
}

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
        s8 sceneId = gEntranceTable[gSaveContext.save.entranceIndex].sceneId;

        // We can safely assume that no cutscene at this point will come from an actor.
        // Do not need to do actor relocation stuff.
        if (entry->sceneId == sceneId && entry->cmp >= 0xFFF0 && entry->cmp == gSaveContext.save.cutsceneIndex &&
            FqCs_CheckSpecialCases(entry)) {
            // TODO: if there are no more special cases in development, just hardcode the intro
            if (entry->csAlt == NULL) {
                gSaveContext.save.cutsceneIndex = 0;
                sQueuedEntry = entry;

                return true;
            }
        }
    }

    return false;
}

void FqCs_ExecuteEntry(PlayState* play, CsHandlerEntry* entry) {
    Player* player = GET_PLAYER(play);

    FqCs_SkipTitleCard(play);

    if (entry->csAlt == NULL) {
        // skip cutscene and run skip func
        gSaveContext.save.cutsceneIndex = 0;
        gSaveContext.cutsceneTrigger = 0;

        if (entry->skipFunc != NULL) {
            entry->skipFunc(play, player);
        }
    } else {
        // play alternate cutscene
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(entry->csAlt);
    }
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
    s32 i;

    if (sQueuedEntry != NULL) {
        FqCs_ExecuteEntry(play, sQueuedEntry);
        sQueuedEntry = NULL;
    } else if (gSaveContext.cutsceneTrigger != 0) {
        // Only search the table if a "non scene layer" cutscene is playing.
        for (i = 0; i < ARRAY_COUNT(gFQCsHandlers); i++) {
            CsHandlerEntry* entry = &gFQCsHandlers[i];
            void* script;

            if (entry->cmp == CMP_NONE) {
                // cutscene is located in the scene, dont need to relocate
                script = SEGMENTED_TO_VIRTUAL(entry->cs);
            } else if (entry->cmp < 0xFFF0) {
                // relocate script pointer for actor, if needed
                ActorOverlay* ovlEntry = &gActorOverlayTable[entry->cmp];
                script = (void*)((u32)entry->cs - (u32)ovlEntry->vramStart + (u32)ovlEntry->loadedRamAddr);
            } else {
                // scene layer script, move on
                continue;
            }

            if (play->csCtx.script == script) {
                FqCs_ExecuteEntry(play, entry);
            }
        }
    }
}
