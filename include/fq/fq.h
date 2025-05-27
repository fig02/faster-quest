#ifndef FQ_H
#define FQ_H

#include "ultra64.h"
#include "fq_player_action.h"

typedef enum QuickTextMode {
    QUICK_TEXT_OFF, // normal text
    QUICK_TEXT_HOLD, // hold B for quick text
    QUICK_TEXT_PRESS // press B for quick text
} QuickTextMode;

typedef struct FasterQuestSettings {
    u8 skipIntro;
    u8 quickTextMode;
} FasterQuestSettings;

typedef struct FasterQuest {
    FasterQuestSettings cfg;
    s32 playerActionRequest; // makes player do a certain action, see FqPlayerActions
} FasterQuest;

void FasterQuest_Init(void);

extern FasterQuest gFQ;

#endif
