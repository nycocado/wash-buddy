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
        /** @brief Ativa chuva pesada e centraliza motores. */
        void enter(GameController* controller) override;

        /** @brief Transiciona para WAITING após timeout. */
        void update(GameController* controller) override;

        /** @brief Para animações de comportamento. */
        void exit(GameController* controller) override;

        /** @brief Valida tags de torneira (repetir) ou toalha (avançar). */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::RINSE. */
        RobotState getStateEnum() const override { return RobotState::RINSE; }

        /** @brief Retorna o tempo configurado para enxágue. */
        unsigned long getTimeout() const override
        {
            return GameConfig::RINSE_TIMEOUT;
        }
};
