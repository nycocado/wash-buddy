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
        /** @brief Centraliza servos e calibra hardware. */
        void enter(GameController* controller) override;

        /** @brief Aguarda a estabilização por tempo fixo. */
        void update(GameController* controller) override;

        /** @brief Sem ações de saída. */
        void exit(GameController* controller) override;

        /** @brief Ignora tags durante a inicialização. */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::BOOT. */
        RobotState getStateEnum() const override { return RobotState::BOOT; }
};
