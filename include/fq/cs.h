#ifndef FQ_CS_H
#define FQ_CS_H

#include "ultra64.h"
#include "z64play.h"
#include "z64player.h"
#include "z64cutscene.h"

typedef void (*CsSkipCallback)(PlayState*,Player*);

typedef struct CsHandlerEntry {
    s8 sceneId;
    s32 cmp; // this can be either a cutscene index, actor id, or neither
    CutsceneData* cs; // optional, used to identify cutscenes that have no cs index
    CutsceneData* csAlt; // optional, alternate cutscene to play instead
    CsSkipCallback skipFunc; // if the cutscene is skipped, function runs to modify player and other game aspects
} CsHandlerEntry;

s32 FqCs_PreSceneOverride(PlayState* play);
void FqCs_Update(PlayState* play);

#endif
