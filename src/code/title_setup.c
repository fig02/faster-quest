#include "console_logo_state.h"
#include "setup_state.h"
#include "printf.h"
#include "translation.h"
#include "save.h"

#include "fq/debug.h"

void Setup_InitImpl(SetupState* this) {
    PRINTF(T("ゼルダ共通データ初期化\n", "Zelda common data initialization\n"));
    SaveContext_Init();
    this->state.running = false;
    // (fq) only go to n64 logo if there is no debug entrance
    if (!DebugFq_LoadToEntrance(&this->state)) {
        SET_NEXT_GAMESTATE(&this->state, ConsoleLogo_Init, ConsoleLogoState);
    }
}

void Setup_Destroy(GameState* thisx) {
}

void Setup_Init(GameState* thisx) {
    SetupState* this = (SetupState*)thisx;

    this->state.destroy = Setup_Destroy;
    Setup_InitImpl(this);
}
