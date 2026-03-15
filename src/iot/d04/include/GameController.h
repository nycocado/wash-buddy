#pragma once

#include "Constants.h"
#include "DisplayOrchestrator.h"
#include "MotionController.h"
#include "RFIDReader.h"
#include "RobotState.h"
#include <Arduino.h>

class State;

/**
 * @class GameController
 * @brief Contexto central da Máquina de Estados Finita (FSM) e gerenciador de
 * hardware.
 *
 * Esta classe é o "cérebro" do Wash-Buddy. Ela coordena a transição entre os
 * diferentes estados do ritual de lavagem de mãos, mantém as referências para
 * todos os controladores de hardware (Display, Movimento, RFID) e aplica as
 * regras de negócio globais.
 *
 * Entre suas responsabilidades estão:
 * - Gerenciar o ciclo de vida dos estados (Enter, Update, Exit).
 * - Controlar o limite de repetições de cada etapa para evitar loops infinitos
 * ou uso indevido.
 * - Prover acesso centralizado ao hardware para os objetos de Estado.
 */
class GameController
{
    public:
        /**
         * @brief Construtor do GameController.
         * @param display Referência para o orquestrador do display OLED.
         * @param motion Referência para o controlador de servos e animações.
         * @param rfid Referência para o leitor de tags RFID.
         */
        GameController(
            DisplayOrchestrator& display,
            MotionController& motion,
            RFIDReader& rfid
        );

        /**
         * @brief Inicializa o controlador e define o estado inicial como BOOT.
         */
        void init();

        /**
         * @brief Loop principal de processamento do controlador.
         * Deve ser chamado repetidamente no loop() principal do Arduino.
         * Verifica a presença de tags RFID e atualiza o estado atual.
         */
        void update();

        /**
         * @brief Altera o estado atual da FSM usando um ponteiro para o objeto
         * de estado.
         * @param newState Ponteiro para a nova instância de Estado.
         */
        void changeState(State* newState);

        /**
         * @brief Altera o estado atual da FSM usando a enumeração de estados.
         * Esta versão contém a lógica de validação de repetições e progresso do
         * ritual.
         * @param stateEnum Identificador do novo estado desejado.
         */
        void changeState(RobotState stateEnum);

        /**
         * @brief Retorna o identificador do estado em execução no momento.
         * @return RobotState Enumeração do estado atual.
         */
        RobotState getCurrentStateEnum() const;

        /**
         * @brief Retorna o último estado do ritual que foi executado com
         * sucesso. Útil para retornar de estados de erro ou espera.
         * @return RobotState O último estado ritualístico registrado.
         */
        RobotState getLastRitualState() const { return _lastRitualState; }

        /**
         * @brief Verifica se um determinado estado faz parte do ritual de
         * lavagem (Molhar, Sabão, etc).
         * @param state O estado a ser verificado.
         * @return true Se for um estado de ritual, false caso contrário.
         */
        bool isRitualState(RobotState state) const;

        /**
         * @brief Converte um valor da enumeração RobotState em uma string
         * legível.
         * @param state O estado para conversão.
         * @return const char* Nome textual do estado.
         */
        const char* getStateName(RobotState state) const;

        /**
         * @brief Tenta repetir o último estado do ritual executado.
         * Dispara a lógica de incremento de repetições.
         */
        void handleRepeat();

        /** @brief Retorna o estado anterior à última transição. */
        State* getPreviousState() const { return _previousState; }

        /** @brief Retorna o timestamp (millis) de quando o estado atual foi
         * iniciado. */
        unsigned long getStateStartTime() const { return _stateStartTime; }

        /** @brief Acesso ao controlador de Display. */
        DisplayOrchestrator& getDisplay() { return _display; }

        /** @brief Acesso ao controlador de Movimento. */
        MotionController& getMotion() { return _motion; }

        /** @brief Acesso ao leitor RFID. */
        RFIDReader& getRFID() { return _rfid; }

    private:
        DisplayOrchestrator& _display; ///< Orquestrador visual
        MotionController& _motion;     ///< Controlador físico (servos)
        RFIDReader& _rfid;             ///< Sensor de entrada (tags)

        State* _currentState;  ///< Estado em execução
        State* _previousState; ///< Estado anterior para referência
        RobotState _lastRitualState =
            RobotState::BOOT; ///< Memória do progresso ritualístico

        unsigned long _stateStartTime; ///< Timestamp de início do estado atual
        int _repeatCount =
            0; ///< Contador de tentativas consecutivas no mesmo estado
        State* _states[11]; ///< Pool de instâncias de estado (Flyweight/Static
                            ///< Allocation)

        /**
         * @brief Aloca e inicializa todas as instâncias de estado no array
         * _states.
         */
        void initializeStates();

        /**
         * @brief Reseta o progresso e contadores do ritual.
         */
        void resetRitualProgress();
};
