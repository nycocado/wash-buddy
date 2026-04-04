#pragma once
#include "State.h"

/**
 * @class DryState
 * @brief Etapa final do ritual: Secagem das mãos.
 *
 * Este estado é ativado quando a criança aproxima a tag da Toalha (Towel).
 * O robô simula a finalização da limpeza através de efeitos de vento e
 * comportamentos de satisfação.
 *
 * Feedback Visual: Efeito de partículas de vento (Wind).
 * Feedback Físico: O robô executa movimentos de cabeça simulando o
 * acompanhamento do uso da toalha.
 */
class DryState : public State
{
    public:
        /**
         * @brief Inicia o efeito de vento e a sonorização de secagem.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Gerencia o tempo de secagem e avança para a celebração final.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Encerra os efeitos de vento e animações de secagem.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Valida se a toalha foi usada corretamente ou se a etapa deve
         * repetir.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::DRY.
         * @return RobotState O estado de secagem.
         */
        RobotState getStateEnum() const override { return RobotState::DRY; }

        /**
         * @brief Retorna o tempo pedagógico configurado para a secagem.
         * @return unsigned long Tempo em milissegundos.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::DRY_TIMEOUT;
        }
};
