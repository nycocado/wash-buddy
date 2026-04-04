#pragma once

#include "AudioTypes.h"
#include <Arduino.h>
#include <DFRobot_DF1201S.h>
#include <HardwareSerial.h>

/**
 * @brief Configurações básicas de hardware e tempo do subsistema de áudio.
 */
struct AudioSettings
{
        uint8_t defaultVolume = 15; ///< Volume inicial ao ligar (0-30)
        uint32_t updateIntervalMs =
            100; ///< Intervalo entre leituras do potenciômetro

        /**
         * @brief Construtor da estrutura de configuração.
         * @param vol Volume inicial desejado.
         * @param interval Tempo em milissegundos entre atualizações de volume.
         */
        AudioSettings(uint8_t vol = 15, uint32_t interval = 100)
            : defaultVolume(vol), updateIntervalMs(interval)
        {
        }
};

/**
 * @class AudioController
 * @brief Interface resiliente para o controle do DFPlayer Pro (DF1201S).
 *
 * Esta classe gerencia a comunicação serial com o módulo de áudio de forma
 * não-bloqueante, utilizando uma máquina de estados interna para garantir a
 * fluidez do sistema e a inicialização assíncrona do hardware.
 */
class AudioController
{
    public:
        /**
         * @enum InitStatus
         * @brief Representa os estágios do ciclo de vida da inicialização do
         * hardware.
         */
        enum class InitStatus
        {
            WAKING_UP,    ///< Aguardando estabilização elétrica do chip (1s)
            CONNECTING,   ///< Tentativa de sincronização via comandos AT da
                          ///< biblioteca
            SETTING_MODE, ///< Configurando modo MUSIC e silenciando avisos de
                          ///< voz
            READY,        ///< Hardware online, configurado e pronto para uso
            ERROR         ///< Falha persistente detectada na comunicação
        };

        /**
         * @brief Construtor do AudioController via Injeção de Dependência.
         *
         * @param serial Referência para a HardwareSerial utilizada (Serial2).
         * @param rxPin Pino de recepção do ESP32 conectado ao TX do player.
         * @param txPin Pino de transmissão do ESP32 conectado ao RX do player.
         * @param potPin Pino analógico (ADC) para o potenciômetro de volume.
         * @param settings Estrutura com as definições iniciais de áudio.
         */
        AudioController(
            HardwareSerial& serial,
            uint8_t rxPin,
            uint8_t txPin,
            uint8_t potPin,
            const AudioSettings& settings = AudioSettings()
        );

        /**
         * @brief Inicializa a porta serial e prepara o estado de boot.
         * Esta função não bloqueia a execução esperando pelo hardware.
         */
        void init();

        /**
         * @brief Processa a máquina de estados e atualiza o volume por
         * hardware. Deve ser chamada continuamente, preferencialmente em task
         * dedicada.
         */
        void update();

        /**
         * @brief Reproduz um arquivo de áudio pela estrutura AudioTrack.
         */
        void playFile(const AudioTrack& track);

        /**
         * @brief Reproduz uma playlist (sequência) de arquivos de áudio.
         * @param maxLoops Número máximo de vezes que a playlist pode tocar (1 =
         * toca apenas uma vez, sem loop).
         */
        void playSequence(const AudioPlaylist& playlist, uint8_t maxLoops = 1);

        /**
         * @brief Reproduz um arquivo em modo de repetição contínua (loop).
         */
        void playLoop(const AudioTrack& track);

        /**
         * @brief Interrompe a reprodução atual.
         */
        void stop();

        /**
         * @brief Define o nível de volume do hardware.
         * @param volume Valor entre 0 (mudo) e 30 (máximo).
         */
        void setVolume(uint8_t volume);

        /**
         * @brief Entra no modo de hibernação, desligando o amplificador.
         */
        void hibernate();

    private:
        /**
         * @enum AudioCommand
         * @brief Comandos internos para evitar conflitos de thread.
         */
        enum class AudioCommand
        {
            NONE,
            PLAY_FILE,
            PLAY_LOOP,
            STOP,
            HIBERNATE
        };

        DFRobot_DF1201S _df1201s; ///< Instância da biblioteca de baixo nível
        HardwareSerial& _serial;  ///< Barramento serial utilizado
        uint8_t _rxPin;           ///< Configuração física do pino RX
        uint8_t _txPin;           ///< Configuração física do pino TX
        uint8_t _potPin; ///< Configuração física do pino do potenciômetro
        AudioSettings _settings; ///< Parâmetros de configuração ativa

        InitStatus _status =
            InitStatus::WAKING_UP; ///< Estado atual da inicialização
        uint8_t _currentVolume;    ///< Cache do volume atual
        unsigned long _lastUpdate; ///< Timestamp da última leitura do ADC
        unsigned long
            _lastAttemptTime; ///< Timestamp da última tentativa de conexão
        unsigned long _bootTimestamp; ///< Timestamp do início do driver

        uint16_t _pendingFile = 0; ///< Armazena track solicitada durante o boot
        bool _hasPendingFile =
            false; ///< Indica se há um comando aguardando prontidão

        AudioCommand _nextCommand =
            AudioCommand::NONE;   ///< Próximo comando a executar
        uint16_t _commandArg = 0; ///< Argumento do comando (ex: index)

        const AudioTrack* _sequenceTracks =
            nullptr;                ///< Ponteiro para a sequência atual
        uint8_t _sequenceCount = 0; ///< Tamanho da sequência
        uint8_t _sequenceIndex = 0; ///< Índice da faixa atual tocando
        uint8_t _sequenceMaxLoops =
            1; ///< Limite máximo de reproduções da playlist
        uint8_t _sequenceCurrentLoop =
            1; ///< Contador de execuções da playlist atual
        unsigned long _lastSequenceCheck =
            0; ///< Temporizador para checar isPlaying() sem travar
        bool _wasPlayingLastCheck =
            false; ///< Guarda o último estado para evitar trocar cedo demais

        unsigned long _lastCommandTime =
            0; ///< Previne sobrecarga de comandos AT no hardware

        /**
         * @brief Lê o potenciômetro e aplica o volume com filtragem de ruído.
         */
        void processVolumePot();

        /**
         * @brief Gerencia as transições de estado para uma inicialização
         * segura.
         */
        void handleInitialization();
};
