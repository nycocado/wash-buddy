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
        /**
         * @brief Configura o pool de comportamentos e ativa a vida orgânica do
         * robô.
         * @param controller Ponteiro para o controlador central.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Gerencia os timers de animações aleatórias e lembretes
         * visuais.
         * @param controller Ponteiro para o controlador central.
         */
        void update(GameController* controller) override;

        /**
         * @brief Limpa o pool de comportamentos ao sair do repouso.
         * @param controller Ponteiro para o controlador central.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Aguarda a tag da torneira para iniciar a jornada pedagógica.
         * @param controller Ponteiro para o controlador central.
         * @param uid Identificador único da tag detectada.
         */
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
