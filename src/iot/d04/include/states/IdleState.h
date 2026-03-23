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
        /** @brief Configura pool de animações e modo Idle dos olhos. */
        void enter(GameController* controller) override;

        /** @brief Gerencia lembretes periódicos e animações aleatórias. */
        void update(GameController* controller) override;

        /** @brief Para animações de comportamento. */
        void exit(GameController* controller) override;

        /** @brief Inicia ritual se tag FAUCET for detectada. */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::IDLE. */
        RobotState getStateEnum() const override { return RobotState::IDLE; }

    private:
        unsigned long _idleNextActionTime =
            0;                ///< Agendamento da próxima animação aleatória
        int _lastAction = -1; ///< Histórico para evitar repetições excessivas
        int _actionStreak = 0;      ///< Contador de ações repetidas
        bool _wasAnimating = false; ///< Estado de animação no frame anterior
        unsigned long _lastReminderTime =
            0; ///< Registro de quando a última instrução foi mostrada
        bool _isPreparingReminder =
            false; ///< Flag para o momento de contato visual
};
