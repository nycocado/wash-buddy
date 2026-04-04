#pragma once

#include "AudioController.h"
#include "BehaviorEngine.h"
#include "Constants.h"
#include "DisplayOrchestrator.h"
#include "MotionController.h"
#include "PowerController.h"
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
 * todos os controladores de hardware (Display, Movimento, RFID, Audio, Power) e
 * aplica as regras de negócio globais.
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
         *
         * Inicializa as referências de hardware e aloca o pool de estados.
         *
         * @param display Referência para o orquestrador do display OLED.
         * @param motion Referência para o controlador de servos e animações.
         * @param rfid Referência para o leitor de tags RFID.
         * @param audio Referência para o controlador de áudio DFPlayer.
         * @param power Referência para o controlador de energia principal.
         */
        GameController(
            DisplayOrchestrator& display,
            MotionController& motion,
            RFIDReader& rfid,
            AudioController& audio,
            PowerController& power
        );

        /**
         * @brief Destrutor para liberação de memória dos estados alocados.
         *
         * Garante que todas as instâncias no array _states sejam deletadas.
         */
        ~GameController();

        /**
         * @brief Inicializa o controlador e define o estado inicial.
         *
         * Configura o estado de BOOT como o ponto de entrada da FSM.
         */
        void init();

        /**
         * @brief Loop principal de processamento do controlador.
         *
         * Deve ser chamado repetidamente no loop() principal do Arduino.
         * Atualiza o estado atual, processa o botão de debug e o motor de
         * comportamento.
         */
        void update();

        /**
         * @brief Processa uma tag RFID lida de forma assíncrona.
         *
         * Delega a lógica de tratamento para o estado atual da FSM.
         *
         * @param uid A string contendo o UID da tag lida.
         */
        void processRFIDTag(const String& uid);

        /**
         * @brief Altera o estado atual da FSM usando um ponteiro direto.
         *
         * @param newState Ponteiro para a nova instância de Estado.
         */
        void changeState(State* newState);

        /**
         * @brief Altera o estado atual da FSM usando a enumeração.
         *
         * Inclui a lógica de validação de repetições permitidas no ritual.
         *
         * @param stateEnum Identificador do novo estado desejado.
         */
        void changeState(RobotState stateEnum);

        /**
         * @brief Retorna o identificador do estado em execução no momento.
         *
         * @return RobotState Enumeração do estado atual.
         */
        RobotState getCurrentStateEnum() const;

        /**
         * @brief Retorna o último estado do ritual que foi executado com
         * sucesso.
         *
         * @return RobotState O último estado ritualístico registrado.
         */
        RobotState getLastRitualState() const { return _lastRitualState; }

        /**
         * @brief Verifica se um determinado estado faz parte do ritual de
         * lavagem.
         *
         * @param state O estado a ser verificado.
         * @return true Se for um estado de ritual (WET, SOAP, etc.), false caso
         * contrário.
         */
        static bool isRitualState(RobotState state);

        /**
         * @brief Converte um valor da enumeração RobotState em uma string
         * legível.
         *
         * @param state O estado para conversão.
         * @return const char* Nome textual do estado (ex: "IDLE").
         */
        static const char* getStateName(RobotState state);

        /**
         * @brief Tenta repetir o último estado do ritual executado.
         *
         * Utilizado quando uma tag de repetição é detectada.
         */
        void handleRepeat();

        /**
         * @brief Retorna o estado anterior à última transição.
         *
         * @return State* Ponteiro para o estado anterior.
         */
        State* getPreviousState() const { return _previousState; }

        /**
         * @brief Retorna o timestamp de quando o estado atual foi iniciado.
         *
         * @return unsigned long Tempo em milissegundos (millis).
         */
        unsigned long getStateStartTime() const { return _stateStartTime; }

        /**
         * @brief Provê acesso ao orquestrador de display.
         *
         * @return DisplayOrchestrator& Referência para o objeto de display.
         */
        DisplayOrchestrator& getDisplay() { return _display; }

        /**
         * @brief Provê acesso ao controlador de movimento.
         *
         * @return MotionController& Referência para o controlador de servos.
         */
        MotionController& getMotion() { return _motion; }

        /**
         * @brief Provê acesso ao leitor RFID.
         *
         * @return RFIDReader& Referência para o objeto de sensor RFID.
         */
        RFIDReader& getRFID() { return _rfid; }

        /**
         * @brief Provê acesso ao controlador de áudio e música.
         *
         * @return AudioController& Referência para o controlador DFPlayer.
         */
        AudioController& getAudio() { return _audio; }

        /**
         * @brief Provê acesso ao controlador de energia.
         *
         * @return PowerController& Referência para o controlador de energia.
         */
        PowerController& getPower() { return _power; }

        /**
         * @brief Provê acesso ao motor de comportamentos aleatórios.
         *
         * @return BehaviorEngine& Referência para o motor de animações
         * orgânicas.
         */
        BehaviorEngine& getBehaviors() { return _behaviors; }

        /**
         * @brief Processa a lógica do botão físico de debug.
         *
         * Detecta pressionamentos curtos e longos para troca de modo e estados.
         */
        void processDebugButton();

        /**
         * @brief Encerra todas as interações e atividades, centraliza os
         * motores e chama o desligamento do sistema no módulo de energia.
         */
        void shutdownSystem();

    private:
        DisplayOrchestrator& _display; ///< Orquestrador visual
        MotionController& _motion;     ///< Controlador físico (servos)
        RFIDReader& _rfid;             ///< Sensor de entrada (tags)
        AudioController& _audio;       ///< Controlador de áudio
        PowerController& _power;       ///< Controlador de energia principal
        BehaviorEngine _behaviors;     ///< Motor de vida orgânica

        State* _currentState;  ///< Estado em execução
        State* _previousState; ///< Estado anterior para referência
        RobotState _lastRitualState =
            RobotState::BOOT; ///< Memória do progresso ritualístico

        unsigned long _stateStartTime; ///< Timestamp de início do estado atual
        int _repeatCount =
            0; ///< Contador de tentativas consecutivas no mesmo estado

        bool _isShuttingDown =
            false; ///< Indica se o sistema está em processo de desligamento

        // As instâncias no pool agora usam o STATE_COUNT dinâmico
        State* _states[static_cast<int>(RobotState::STATE_COUNT)];

        // Variáveis de controle de Debug
        bool _isDebugMode = false;
        unsigned long _buttonPressTime = 0;
        bool _buttonWasPressed = false;
        bool _debugToggleHandled = false;
        unsigned long _debugTextClearTime =
            0; ///< Timer para limpar mensagens temporárias da tela

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
