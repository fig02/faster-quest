#include "z64actor.h"
#include "z64player.h"
#include "z64play.h"
#include "printf.h"

#include "fq/fq.h"

typedef void (*FqPlayerSetupActionFunc)(struct Player*, struct PlayState*);

void Player_Action_Idle(Player* this, PlayState* play);

void PlayerFq_SetupIdle(Player* this, PlayState* play) {
    Player_SetupAction(play, this, Player_Action_Idle, 1);
    Player_AnimPlayOnce(play, this, Player_GetIdleAnim(this));
}

FqPlayerSetupActionFunc sSetupActionTable[] = {
    PlayerFq_SetupIdle, // FQ_PLAYER_ACTION_IDLE
};

void PlayerFq_ProcessActionRequests(Player* this, PlayState* play) {
    if (this->actor.category == ACTORCAT_PLAYER && gFQ.playerActionRequest != FQ_PLAYER_ACTION_NONE) {
        FqPlayerSetupActionFunc setupAction = sSetupActionTable[gFQ.playerActionRequest];
        PRINTF("!!!!! FUNC %8X\n", setupAction);
        setupAction(this, play);
        gFQ.playerActionRequest = FQ_PLAYER_ACTION_NONE;
    }
}
