#pragma once

#include "Constants.h"
#include "RobotState.h"
#include <Arduino.h>

class GameController;

/**
 * @class State
 * @brief Interface base para a implementação dos estados do robô.
 *
 * Define o contrato que todos os estados concretos devem seguir, utilizando o
 * padrão de projeto State para desacoplar a lógica de comportamento.
 */
class State
{
    public:
        /** @brief Destrutor virtual para limpeza das subclasses. */
        virtual ~State() = default;

        /**
         * @brief Chamado uma única vez ao entrar neste estado.
         * @param controller Ponteiro para o contexto principal da FSM.
         */
        virtual void enter(GameController* controller) = 0;

        /**
         * @brief Chamado repetidamente a cada ciclo do loop principal.
         * @param controller Ponteiro para o contexto principal da FSM.
         */
        virtual void update(GameController* controller) = 0;

        /**
         * @brief Chamado uma única vez ao sair deste estado.
         * @param controller Ponteiro para o contexto principal da FSM.
         */
        virtual void exit(GameController* controller) = 0;

        /**
         * @brief Processa a detecção de uma tag RFID.
         * @param controller Ponteiro para o contexto principal da FSM.
         * @param uid Identificador único da tag lida pelo sensor.
         */
        virtual void
        handleRFID(GameController* controller, const String& uid) = 0;

        /**
         * @brief Retorna o identificador único deste estado.
         * @return O valor da enumeração correspondente ao estado.
         */
        virtual RobotState getStateEnum() const = 0;

        /**
         * @brief Retorna o tempo de duração permitido para este estado.
         * @return Tempo limite em milissegundos.
         */
        virtual unsigned long getTimeout() const
        {
            return GameConfig::STATE_TIMEOUT_DEFAULT;
        }

    protected:
        /**
         * @brief Função auxiliar para padronizar a validação de tags RFID.
         * @param controller Contexto da FSM.
         * @param uid UID da tag lida.
         * @param currentTag UID esperado para repetir o estado atual.
         * @param nextTag UID esperado para avançar no ritual.
         * @param nextState Estado de destino em caso de avanço.
         */
        void validateRFID(
            GameController* controller,
            const String& uid,
            const char* currentTag,
            const char* nextTag,
            RobotState nextState
        );
};
