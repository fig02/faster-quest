#ifndef FQ_H
#define FQ_H

#include "ultra64.h"

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
} FasterQuest;

void FasterQuest_Init(void);

extern FasterQuest gFQ;

#endif
