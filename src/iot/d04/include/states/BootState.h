#pragma once
#include "State.h"

/**
 * @class BootState
 * @brief Estado inicial de inicialização do robô.
 *
 * Este estado é executado assim que o robô é ligado. Sua função é garantir
 * que todos os atuadores (servos) estejam em uma posição segura e centralizada
 * antes de iniciar a interação com a criança.
 */
class BootState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::BOOT; }
};
