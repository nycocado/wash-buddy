#pragma once
#include "State.h"

/**
 * @brief Representa a fase final de secagem das mãos.
 *
 * Ativado quando o usuário aproxima a toalha do robô.
 * Conclui o processo de limpeza antes da celebração.
 *
 * Feedback Visual: Expressão de admiração (Awe) e efeito de partículas de
 * vento. Feedback Físico: O robô olha para a esquerda, simulando a direção da
 * toalha.
 */
class DryState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::DRY; }
        unsigned long getTimeout() const override
        {
            return GameConfig::DRY_TIMEOUT;
        }
};
