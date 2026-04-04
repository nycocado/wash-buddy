#pragma once
#include "State.h"

/**
 * @class ScrubState
 * @brief Etapa ativa de esfregação das mãos.
 *
 * Esta é a fase central do ritual, ativada após a aplicação do sabão.
 * O robô incentiva a criança a esfregar as mãos pelo tempo recomendado pelas
 * autoridades de saúde.
 *
 * Feedback Visual: Manutenção das bolhas no display.
 * Feedback Físico: Coreografia rítmica de esfregação com ambos os braços.
 */
class ScrubState : public State
{
    public:
        /**
         * @brief Inicia a coreografia de esfregação e sonorização rítmica.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Monitora o tempo obrigatório de higiene e transiciona para
         * espera.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Para as animações de esfregação.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Valida se a criança continua a esfregação ou avança para o
         * enxágue.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::SCRUB.
         * @return RobotState O estado de esfregação.
         */
        RobotState getStateEnum() const override { return RobotState::SCRUB; }

        /**
         * @brief Retorna o tempo pedagógico (OMS) para esfregação eficiente.
         * @return unsigned long Tempo em milissegundos.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::SCRUB_TIMEOUT;
        }
};
