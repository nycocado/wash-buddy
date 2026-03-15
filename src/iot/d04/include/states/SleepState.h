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
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::SLEEP; }
};
