#pragma once
#include "State.h"

/**
 * @brief Gerencia a fase ativa de esfregação.
 *
 * Ativado quando o usuário aproxima a esponja após o sabão.
 * É a parte central do ritual onde o robô simula o ato de esfregar as mãos.
 *
 * Feedback Visual: Expressão alegre para incentivar o usuário e manutenção das
 * bolhas. Feedback Físico: Execução de coreografias nos braços simulando
 * movimento de esfregação.
 */
class ScrubState : public State
{
    public:
        /** @brief Ativa bolhas e inicia coreografia de esfregação. */
        void enter(GameController* controller) override;

        /** @brief Transiciona para WAITING após timeout. */
        void update(GameController* controller) override;

        /** @brief Para animações de comportamento. */
        void exit(GameController* controller) override;

        /** @brief Valida tags de esfregação (repetir) ou torneira (avançar). */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::SCRUB. */
        RobotState getStateEnum() const override { return RobotState::SCRUB; }

        /** @brief Retorna o tempo configurado para esfregação. */
        unsigned long getTimeout() const override
        {
            return GameConfig::SCRUB_TIMEOUT;
        }
};
