#pragma once
#include "State.h"

/**
 * @brief Estado de celebração ao concluir o ritual corretamente.
 *
 * Ativado após o sucesso na fase de secagem.
 * O objetivo é recompensar o usuário e reforçar o hábito positivo.
 *
 * Feedback Visual: Expressão de extrema felicidade (Glee),
 * animação de vibração e partículas de confete.
 * Feedback Físico: Acena as mãos em comemoração.
 */
class SuccessState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::SUCCESS; }
        unsigned long getTimeout() const override
        {
            return GameConfig::SUCCESS_DISPLAY;
        }
};
