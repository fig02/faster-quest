#ifndef FQ_H
#define FQ_H

#include "ultra64.h"

typedef enum QuickTextMode {
    QUICK_TEXT_OFF,
    QUICK_TEXT_HOLD,
    QUICK_TEXT_PRESS
} QuickTextMode;

typedef struct FasterQuestSettings {
    u8 skipIntro;
    u8 quickTextMode;
} FasterQuestSettings;

extern FasterQuestSettings gFQ;

#endif
