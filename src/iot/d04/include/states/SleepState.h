#pragma once
#include "State.h"

/**
 * @brief Estado de baixo consumo de energia do robô.
 *
 * Ativado após um longo período de inatividade em IDLE.
 * O objetivo é economizar bateria desligando feedbacks ativos.
 *
 * Feedback Visual: Display desligado ou animação de olhos fechados.
 * Feedback Físico: Motores centralizados e desativados para evitar consumo.
 */
class SleepState : public State
{
    public:
        /** @brief Centraliza motores antes do repouso profundo. */
        void enter(GameController* controller) override;

        /** @brief Mantém o sistema em baixo consumo. */
        void update(GameController* controller) override;

        /** @brief Sem ações de saída. */
        void exit(GameController* controller) override;

        /** @brief RFID pode ser usado para acordar o sistema. */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::SLEEP. */
        RobotState getStateEnum() const override { return RobotState::SLEEP; }
};
