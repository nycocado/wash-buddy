#pragma once
#include "State.h"

/**
 * @brief Estado ativado quando uma ação incorreta é detectada no ritual.
 *
 * Este estado interrompe o fluxo normal quando o usuário usa um item fora de
 * ordem ou não esperado para a fase atual do ritual.
 *
 * Feedback Visual: Expressão cética (Skeptic) e animação de confusão no
 * display. Feedback Físico: O robô nega com a cabeça (shake head) e levanta os
 * braços em sinal de dúvida.
 */
class ErrorState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::ERROR; }
};
