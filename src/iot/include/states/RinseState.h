#pragma once
#include "State.h"

/**
 * @class RinseState
 * @brief Etapa de enxágue das mãos.
 *
 * Ativado quando a criança aproxima a tag da Torneira (Faucet) após a etapa de
 * esfregação. Simula a remoção do sabão sob água corrente.
 *
 * Feedback Visual: Efeito de chuva forte (Heavy Rain).
 * Feedback Físico: O robô executa movimentos de 'mergulho' com os braços,
 * simulando a posição das mãos sob a água.
 */
class RinseState : public State
{
    public:
        /**
         * @brief Ativa o efeito de chuva pesada e sonorização de enxágue.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Monitora o tempo de enxágue e transiciona para espera após
         * conclusão.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Limpa animações e efeitos de enxágue.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Valida tags de torneira (repetir) ou toalha (avançar).
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::RINSE.
         * @return RobotState O estado de enxágue.
         */
        RobotState getStateEnum() const override { return RobotState::RINSE; }

        /**
         * @brief Retorna o tempo pedagógico configurado para o enxágue.
         * @return unsigned long Tempo em milissegundos.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::RINSE_TIMEOUT;
        }
};
