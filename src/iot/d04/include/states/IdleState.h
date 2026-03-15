#pragma once
#include "State.h"

/**
 * @class IdleState
 * @brief Estado de espera ativa, onde o robô aguarda o início do ritual.
 *
 * Neste estado, o robô demonstra "vida" através de movimentos aleatórios de
 * cabeça, acenos e expressões faciais. O objetivo é atrair a atenção da criança
 * e sinalizar que ele está pronto para começar. O ritual começa quando a tag da
 * torneira (Faucet) é detectada.
 */
class IdleState : public State
{
    public:
        void enter(GameController* controller) override;
        void update(GameController* controller) override;
        void exit(GameController* controller) override;
        void handleRFID(GameController* controller, const String& uid) override;
        RobotState getStateEnum() const override { return RobotState::IDLE; }

    private:
        unsigned long _idleNextActionTime =
            0;                ///< Agendamento da próxima animação aleatória
        int _lastAction = -1; ///< Histórico para evitar repetições excessivas
        int _actionStreak = 0;      ///< Contador de ações repetidas
        bool _wasAnimating = false; ///< Estado de animação no frame anterior
};
