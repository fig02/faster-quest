#ifndef FQ_CS_H
#define FQ_CS_H

#include "ultra64.h"
#include "z64play.h"
#include "z64player.h"
#include "z64cutscene.h"

typedef s32 (*CsCheck)(PlayState*);
typedef void (*CsModifier)(PlayState*,Player*);

typedef struct CsHandlerEntry {
    s32 entranceIndex;
    s32 cutsceneIndex;
    CutsceneData* cs; // optional, used to identify cutscenes that have no cs index
    CutsceneData* alternateCs; // optional, alternate cs used for QUICK_CS_SHORTEN
    CsCheck check; // returns true if the cutscene should be skipped
    CsModifier modifier; // if the cutscene is skipped, function runs to modify player and other game aspects
} CsHandlerEntry;

s32 FqCs_PreSceneOverride(PlayState* play);
void FqCs_Update(PlayState* play);

#endif
