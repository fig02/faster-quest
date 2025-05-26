#include "fq/fq.h"

FasterQuestSettings gFQDefaultSettings = {
    true,            // skipIntro
    QUICK_TEXT_HOLD, // quickTextMode
};

FasterQuest gFQ;

void FasterQuest_Init(void) {
    gFQ.cfg = gFQDefaultSettings;
}
