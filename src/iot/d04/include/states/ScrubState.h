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
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::SCRUB; }
        unsigned long getTimeout() const override
        {
            return GameConfig::SCRUB_TIMEOUT;
        }
};
