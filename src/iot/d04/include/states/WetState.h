#pragma once
#include "State.h"

/**
 * @class WetState
 * @brief Primeira etapa do ritual: Molhar as mãos.
 *
 * Este estado é ativado quando a criança aproxima a tag da Torneira (Faucet)
 * enquanto o robô está em Idle. O robô reage com surpresa e exibe um efeito
 * de chuva leve no display para simular a água.
 */
class WetState : public State
{
    public:
        /** @brief Ativa chuva leve e prepara servos. */
        void enter(GameController* controller) override;

        /** @brief Transiciona para WAITING após timeout. */
        void update(GameController* controller) override;

        /** @brief Para animações de comportamento. */
        void exit(GameController* controller) override;

        /** @brief Valida tags de torneira (repetir) ou sabão (avançar). */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::WET. */
        RobotState getStateEnum() const override { return RobotState::WET; }

        /** @brief Retorna o tempo configurado para molhar as mãos. */
        unsigned long getTimeout() const override
        {
            return GameConfig::WET_TIMEOUT;
        }
};
