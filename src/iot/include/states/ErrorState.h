#pragma once
#include "State.h"

/**
 * @class ErrorState
 * @brief Estado de atenção disparado por ações incorretas ou fora de sequência.
 *
 * Este estado interrompe o fluxo normal quando a criança utiliza um item que
 * não é o esperado para a fase atual do ritual (ex: tentar usar a toalha logo
 * após o sabão). O robô demonstra confusão para sinalizar a necessidade de
 * atenção.
 *
 * Feedback Visual: Animação de confusão nos olhos.
 * Feedback Físico: O robô nega com a cabeça e levanta os braços.
 */
class ErrorState : public State
{
    public:
        /**
         * @brief Dispara o áudio de erro e ativa a expressão de dúvida.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Aguarda o fim do feedback de erro e retorna ao estado
         * adequado.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Limpa o estado de erro e libera o comportamento.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Ignora novas leituras enquanto a mensagem de erro está ativa.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::ERROR.
         * @return RobotState O estado de erro.
         */
        RobotState getStateEnum() const override { return RobotState::ERROR; }
};
