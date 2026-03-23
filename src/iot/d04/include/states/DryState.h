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
        /** @brief Inicia efeito de vento e expressão de admiração. */
        void enter(GameController* controller) override;

        /** @brief Transiciona para SUCCESS após timeout. */
        void update(GameController* controller) override;

        /** @brief Para animações de comportamento. */
        void exit(GameController* controller) override;

        /** @brief Valida tag de toalha (TOWEL). */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::DRY. */
        RobotState getStateEnum() const override { return RobotState::DRY; }

        /** @brief Retorna o tempo configurado para secagem. */
        unsigned long getTimeout() const override
        {
            return GameConfig::DRY_TIMEOUT;
        }
};
