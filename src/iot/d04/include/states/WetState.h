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
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::WET; }

        /** @brief Retorna o tempo configurado para molhar as mãos. */
        unsigned long getTimeout() const override
        {
            return GameConfig::WET_TIMEOUT;
        }
};
