#ifndef FQ_H
#define FQ_H

#include "ultra64.h"

typedef enum QuickTextMode {
    QUICK_TEXT_OFF, // normal text
    QUICK_TEXT_HOLD, // hold B for quick text
    QUICK_TEXT_PRESS // press B for quick text
} QuickTextMode;

typedef enum QuickCsType {
    QUICK_CS_OFF, // normal cutscenes
    QUICK_CS_SHORTEN, // alternate version of cutscene play which are much shorter
    QUICK_CS_SKIP // cutscene is skipped entirely
} QuickCsType;

typedef struct FasterQuestSettings {
    u8 skipIntro;
    u8 quickTextMode;
    u8 quickCsType;
} FasterQuestSettings;

extern FasterQuestSettings gFQ;

#endif
