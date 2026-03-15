#pragma once

#include "Constants.h"
#include "RobotState.h"
#include <Arduino.h>

class GameController;

/**
 * @class State
 * @brief Interface base (abstrata) para a implementação dos estados do robô.
 *
 * Define o contrato que todos os estados concretos (Boot, Idle, etc.) devem
 * seguir. Utiliza o padrão de projeto State para desacoplar a lógica de
 * comportamento do contexto principal (GameController).
 */
class State
{
    public:
        /** @brief Destrutor virtual para garantir a limpeza correta das
         * subclasses. */
        virtual ~State() = default;

        /**
         * @brief Método chamado uma única vez quando a FSM entra neste estado.
         * Usado para configurar animações iniciais, sons ou LEDs.
         * @param controller Ponteiro para o contexto principal.
         */
        virtual void enter(GameController* controller) = 0;

        /**
         * @brief Método chamado repetidamente a cada ciclo do loop principal.
         * Usado para verificar timeouts, atualizar animações ou lógica
         * temporal.
         * @param controller Ponteiro para o contexto principal.
         */
        virtual void update(GameController* controller) = 0;

        /**
         * @brief Método chamado uma única vez quando a FSM sai deste estado.
         * Usado para limpar recursos ou parar animações.
         * @param controller Ponteiro para o contexto principal.
         */
        virtual void exit(GameController* controller) = 0;

        /**
         * @brief Processa a detecção de uma tag RFID.
         * Cada estado decide como reagir a diferentes tags (avançar, repetir ou
         * erro).
         * @param controller Ponteiro para o contexto principal.
         * @param uid Identificador único da tag lida.
         */
        virtual void
        handleRFID(GameController* controller, const String& uid) = 0;

        /**
         * @brief Retorna o identificador único (enum) deste estado.
         * @return RobotState O enum correspondente ao estado.
         */
        virtual RobotState getStateEnum() const = 0;

        /**
         * @brief Retorna o tempo de duração (timeout) permitido para este
         * estado. Por padrão, utiliza o timeout global definido em GameConfig.
         * @return unsigned long Tempo em milissegundos.
         */
        virtual unsigned long getTimeout() const
        {
            return GameConfig::STATE_TIMEOUT_DEFAULT;
        }

    protected:
        /**
         * @brief Função auxiliar para padronizar a validação de tags RFID entre
         * os estados. Verifica se a tag lida corresponde à tag atual
         * (repetição), à próxima tag (avanço) ou se é uma tag inválida (erro).
         *
         * @param controller Contexto da FSM.
         * @param uid UID da tag lida.
         * @param currentTag UID esperado para repetir o estado atual.
         * @param nextTag UID esperado para avançar para o próximo estado.
         * @param nextState O estado para o qual avançar se nextTag for lida.
         */
        void validateRFID(
            GameController* controller,
            const String& uid,
            const char* currentTag,
            const char* nextTag,
            RobotState nextState
        );
};
