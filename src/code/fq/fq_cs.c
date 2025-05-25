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

void FqCs_SariaIntroSkip(PlayState* play, Player* player) {
    FqCs_SetPlayerPosYaw(player, -30.0f, 100.0f, 1025.0f, 0x8000, true);
}

void FqCs_DekuTreeIntroSkip(PlayState* play, Player* player) {
    Actor* treemouth = Actor_Find(&play->actorCtx, ACTOR_BG_TREEMOUTH, ACTORCAT_BG);
    Audio_PlaySfxGeneral(NA_SE_EV_WOODDOOR_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    gFQ.cs.cue = true;
}

// ================= TABLE

// scene cutscenes
#include "assets/scenes/overworld/spot04/spot04_scene.h"
#include "assets/scenes/indoors/link_home/link_home_scene.h"

// alternate cutscenes
#include "assets/scenes/kokiri_forest/kokiri_forest.h"

// actor cutscenes
extern CutsceneData gDekuTreeMeetingCs[];

#define CMP_NONE -1
#define ALT_CS_NONE NULL

static CsHandlerEntry gFQCsHandlers[] = {
    { SCENE_LINKS_HOUSE, 0xFFF1, gLinkHouseIntroSleepCs, ALT_CS_NONE, FqCs_IntroSkip },
    { SCENE_KOKIRI_FOREST, CMP_NONE, gKokiriForestSariaGreetingCs, gAltKokiriForestSariaGreetingCs,
      FqCs_SariaIntroSkip },
    { SCENE_KOKIRI_FOREST, ACTOR_BG_TREEMOUTH, gDekuTreeMeetingCs, ALT_CS_NONE, FqCs_DekuTreeIntroSkip },
};

static CsHandlerEntry* sQueuedEntry = NULL;

s32 FqCs_CheckSpecialCases(CsHandlerEntry* entry) {
    if (entry->sceneId == SCENE_LINKS_HOUSE && entry->cmp == 0xFFF1) {
        // Check FQ settings for intro skip
        return gFQ.cfg.skipIntro;
    }

    // Default to skipping cutscene if there are no special cases
    return true;
}

#define FQ_SHOULD_SKIP_CS(entry) \
    (gFQ.cfg.quickCsType == QUICK_CS_SKIP || (gFQ.cfg.quickCsType == QUICK_CS_SHORTEN && entry->csAlt == NULL))

/**
 * This function runs early in the Play_Init process to override the scene layer if necessary.
 * It also sets cutsceneIndex to zero to skip the cutscene.
 *
 * This only applies to scene layer cutscenes that start right away on scene load.
 */
s32 FqCs_PreSceneOverride(PlayState* play) {
    s32 i;

    if (gFQ.cfg.quickCsType == QUICK_CS_OFF) {
        return false;
    }

    for (i = 0; i < ARRAY_COUNT(gFQCsHandlers); i++) {
        CsHandlerEntry* entry = &gFQCsHandlers[i];
        s8 sceneId = gEntranceTable[gSaveContext.save.entranceIndex].sceneId;

        // We can safely assume that no cutscene at this point will come from an actor.
        // Do not need to do actor relocation stuff.
        if (entry->sceneId == sceneId && entry->cmp >= 0xFFF0 && entry->cmp == gSaveContext.save.cutsceneIndex &&
            FqCs_CheckSpecialCases(entry)) {
            if (FQ_SHOULD_SKIP_CS(entry)) {
                gSaveContext.save.cutsceneIndex = 0;
                sQueuedEntry = entry;
                return true;
            }
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

    // clear the force cue that may have been set on the last frame
    gFQ.cs.cue = false;

    if (gFQ.cfg.quickCsType == QUICK_CS_OFF) {
        return;
    }

    if (sQueuedEntry != NULL) {
        FqCs_SkipTitleCard(play);

        if (FQ_SHOULD_SKIP_CS(sQueuedEntry)) {
            // If there was a match in the PreSceneOverride check, it was saved in `sQueuedEntry`.
            // This prevents having to search through the table again to find it.
            sQueuedEntry->skipFunc(play, player);
            sQueuedEntry = NULL;
        } else if (gFQ.cfg.quickCsType == QUICK_CS_SHORTEN) {
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(sQueuedEntry->csAlt);
        }
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
                FqCs_SkipTitleCard(play);

                if (FQ_SHOULD_SKIP_CS(entry)) {
                    gSaveContext.save.cutsceneIndex = 0;
                    gSaveContext.cutsceneTrigger = 0;
                    entry->skipFunc(play, player);
                } else if (gFQ.cfg.quickCsType == QUICK_CS_SHORTEN) {
                    play->csCtx.script = SEGMENTED_TO_VIRTUAL(entry->csAlt);
                }
            }
        }
    }
}
