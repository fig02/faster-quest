#include "fq/fq.h"

FasterQuestSettings gFQDefaultSettings = {
    true,             // skipIntro
    QUICK_TEXT_HOLD,  // quickTextMode
    QUICK_CS_SHORTEN, // quickCsType
};

FasterQuest gFQ;

void FasterQuest_Init(void) {
    gFQ.cfg = gFQDefaultSettings;
}
