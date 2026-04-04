#pragma once
#include "State.h"

/**
 * @class SuccessState
 * @brief Estado de celebração ao concluir o ritual de lavagem corretamente.
 *
 * Este estado é ativado após o sucesso na fase de secagem. O objetivo é
 * recompensar a criança e reforçar positivamente o hábito de higiene.
 *
 * Feedback Visual: Expressão de extrema felicidade (Glee) e partículas de
 * confete. Feedback Físico: O robô acena os braços em sinal de vitória.
 */
class SuccessState : public State
{
    public:
        /**
         * @brief Ativa os confetes, o áudio de vitória e o humor festivo.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Aguarda o fim da celebração e transiciona para o desligamento.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Encerra as animações de sucesso.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Ignora novas leituras durante a comemoração final.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::SUCCESS.
         * @return RobotState O estado de sucesso.
         */
        RobotState getStateEnum() const override { return RobotState::SUCCESS; }

        /**
         * @brief Retorna o tempo de exibição da celebração.
         * @return unsigned long Tempo em milissegundos.
         */
        unsigned long getTimeout() const override
        {
            return GameConfig::SUCCESS_DISPLAY_MS;
        }
};
