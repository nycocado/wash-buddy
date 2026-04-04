#pragma once
#include "State.h"

/**
 * @class WetState
 * @brief Primeira etapa ativa do ritual: Molhar as mãos.
 *
 * Este estado é ativado quando a criança aproxima a tag da Torneira (Faucet)
 * enquanto o robô está em Idle. O objetivo é preparar as mãos para o sabão.
 *
 * Feedback Visual: Efeito de chuva leve (Light Rain).
 * Feedback Físico: O robô executa movimentos de 'mergulho' simulando a
 * interação com a água.
 */
class WetState : public State
{
    public:
        /**
         * @brief Ativa o efeito de chuva leve e inicia a sonorização de água.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Gerencia o tempo de exposição à água e transiciona para
         * espera.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Encerra as animações e efeitos de água.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Valida tags de torneira (repetir) ou sabão (avançar).
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::WET.
         * @return RobotState O estado de molhar as mãos.
         */
        RobotState getStateEnum() const override { return RobotState::WET; }

        /**
         * @brief Retorna o tempo pedagógico configurado para molhar as mãos.
         * @return unsigned long Tempo em milissegundos.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::WET_TIMEOUT;
        }
};
