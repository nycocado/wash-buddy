#pragma once
#include "State.h"

/**
 * @brief Gerencia a etapa de enxágue do ritual.
 *
 * Ativado quando o usuário "abre a torneira" (aproxima o tag da torneira).
 * Simula a remoção do sabão sob água corrente.
 *
 * Feedback Visual: Olhos semifechados (como se protegesse da água),
 * olhar voltado para baixo e efeito de chuva pesada para simular água.
 * Feedback Físico: Motores retornam ao centro para simular estabilidade sob a
 * água.
 */
class RinseState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::RINSE; }
        unsigned long getTimeout() const override
        {
            return GameConfig::RINSE_TIMEOUT;
        }
};
