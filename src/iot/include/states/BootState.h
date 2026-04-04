#pragma once
#include "State.h"

/**
 * @class BootState
 * @brief Estado inicial de inicialização e calibração do robô.
 *
 * Este estado é o ponto de entrada da FSM logo após o power-on. Sua função
 * principal é garantir que todos os atuadores (servos) estejam em uma posição
 * segura e centralizada, além de aguardar a prontidão do hardware de áudio.
 */
class BootState : public State
{
    public:
        /**
         * @brief Centraliza servos e inicia a sequência de áudio de boot.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void enter(GameController* controller) override;

        /**
         * @brief Aguarda a estabilização elétrica e lógica do hardware.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void update(GameController* controller) override;

        /**
         * @brief Finaliza as rotinas de boot.
         * @param controller Ponteiro para o controlador central de jogo.
         */
        void exit(GameController* controller) override;

        /**
         * @brief Ignora qualquer leitura de tag durante o processo de boot.
         * @param controller Ponteiro para o controlador de jogo.
         * @param uid Identificador da tag detectada.
         */
        void handleRFID(GameController* controller, const String& uid) override;

        /**
         * @brief Retorna o enum RobotState::BOOT.
         * @return RobotState O estado de boot.
         */
        RobotState getStateEnum() const override { return RobotState::BOOT; }
};
