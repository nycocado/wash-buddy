#pragma once
#include "State.h"

/**
 * @class WaitingState
 * @brief Estado de espera por ação durante o ritual.
 *
 * Este estado é acionado quando uma etapa do ritual termina e o robô aguarda
 * que a criança aproxime a próxima tag RFID (ex: após molhar, espera o sabão).
 * Se a espera for muito longa (timeout), o robô assume que o ritual foi
 * abandonado e retorna para IdleState.
 *
 * O robô demonstra preocupação crescente conforme o tempo passa, mudando de
 * expressões "preocupadas" para "tristes" e pedindo ajuda com os braços.
 */
class WaitingState : public State
{
    public:
        /** @brief Inicia comportamento de espera e mostra ícone da próxima
         * etapa. */
        void enter(GameController* controller) override;

        /** @brief Gerencia evolução da impaciência e timeout de abandono. */
        void update(GameController* controller) override;

        /** @brief Para animações de comportamento. */
        void exit(GameController* controller) override;

        /** @brief Valida se a tag aproxima corresponde ao próximo passo
         * ritualístico. */
        void handleRFID(GameController* controller, const String& uid) override;

        /** @brief Retorna RobotState::WAITING. */
        RobotState getStateEnum() const override { return RobotState::WAITING; }

    private:
        unsigned long _nextActionTime =
            0; ///< Controle de tempo para animações de "impaciência"
        bool _isSadPhase =
            false; ///< Indica se entrou na fase de tristeza por demora
};
