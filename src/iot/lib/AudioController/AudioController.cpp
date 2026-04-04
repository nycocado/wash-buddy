#include "AudioController.h"

/** @section Ciclo de Vida e Inicialização */

/**
 * @brief Construtor da classe de controle de áudio.
 */
AudioController::AudioController(
    HardwareSerial& serial,
    uint8_t rxPin,
    uint8_t txPin,
    uint8_t potPin,
    const AudioSettings& settings
)
    : _serial(serial), _rxPin(rxPin), _txPin(txPin), _potPin(potPin),
      _settings(settings), _currentVolume(settings.defaultVolume),
      _lastUpdate(0), _lastAttemptTime(0), _bootTimestamp(0), _pendingFile(0),
      _hasPendingFile(false)
{
}

/**
 * @brief Configuração inicial de hardware e comunicação.
 *
 * Inicia a porta Serial2 mas não bloqueia a execução esperando o hardware.
 */
void AudioController::init()
{
    // Prepara o pino do potenciômetro para leitura analógica
    pinMode(_potPin, INPUT);

    // Inicializa a HardwareSerial com o baud rate padrão do DFPlayer Pro
    // (115200)
    _serial.begin(115200, SERIAL_8N1, _rxPin, _txPin);

    // Inicia a contagem de tempo para a máquina de estados assíncrona
    _bootTimestamp = millis();
    _status = InitStatus::WAKING_UP;

    Serial.println(F(
        "[AUDIO] Orquestrador de som carregado e aguardando boot do hardware."
    ));
}

/**
 * @brief Atualização periódica do subsistema de áudio.
 *
 * Gerencia a máquina de estados de inicialização resiliente e a leitura do
 * potenciômetro de volume com controle de cadência.
 */
void AudioController::update()
{
    // Enquanto não estiver pronto, processa exclusivamente a sequência de
    // inicialização
    if (_status != InitStatus::READY)
    {
        handleInitialization();
        return;
    }

    unsigned long now = millis();

    // Processamento do volume via potenciômetro com controle de cadência
    if (now - _lastUpdate > _settings.updateIntervalMs)
    {
        _lastUpdate = now;
        processVolumePot();
    }

    // --- PROCESSAMENTO DE COMANDOS (Thread-Safe) ---
    // Executamos apenas um comando de hardware por ciclo de update para evitar
    // saturação do barramento serial.
    if (_nextCommand != AudioCommand::NONE)
    {
        switch (_nextCommand)
        {
            case AudioCommand::PLAY_FILE:
                _df1201s.setPlayMode(DFRobot_DF1201S::SINGLE);
                _df1201s.playFileNum(_commandArg);
                break;
            case AudioCommand::PLAY_LOOP:
                _df1201s.setPlayMode(DFRobot_DF1201S::SINGLECYCLE);
                _df1201s.playFileNum(_commandArg);
                break;
            case AudioCommand::STOP:
                _df1201s.pause();
                break;
            case AudioCommand::HIBERNATE:
                _df1201s.pause();
                _df1201s.disableAMP();
                break;
            default:
                break;
        }
        _nextCommand = AudioCommand::NONE;
        _wasPlayingLastCheck = true;
        return; // Sai para dar tempo ao hardware de processar antes do
                // isPlaying()
    }

    // Lógica de sequência de áudio (Playlist)
    if (_sequenceTracks != nullptr)
    {
        // Checamos isPlaying apenas a cada 300ms para não travar a porta serial
        if (now - _lastSequenceCheck > 300)
        {
            _lastSequenceCheck = now;
            bool isPlayingNow = _df1201s.isPlaying();

            // Se estava tocando e agora não está mais, passou para a próxima
            // faixa
            if (_wasPlayingLastCheck && !isPlayingNow)
            {
                _sequenceIndex++;
                if (_sequenceIndex < _sequenceCount)
                {
                    _df1201s.playFileNum(_sequenceTracks[_sequenceIndex].id);
                    _wasPlayingLastCheck = true;
                }
                else
                {
                    if (_sequenceCurrentLoop < _sequenceMaxLoops)
                    {
                        // Incrementa o contador de loops e reinicia a playlist
                        _sequenceCurrentLoop++;
                        _sequenceIndex = 0;
                        _df1201s.playFileNum(_sequenceTracks[0].id);
                        _wasPlayingLastCheck = true;
                    }
                    else
                    {
                        _sequenceTracks = nullptr; // Fim da playlist
                    }
                }
            }
            else
            {
                _wasPlayingLastCheck = isPlayingNow;
            }
        }
    }
}

/**
 * @section Lógica de Inicialização Resiliente
 * Segue o protocolo sugerido pelo fabricante: Power-on -> Handshake -> Mode
 * Switch -> Ready.
 */
void AudioController::handleInitialization()
{
    unsigned long now = millis();

    switch (_status)
    {
        case InitStatus::WAKING_UP:
            // Aguarda 1000ms para estabilização elétrica total do módulo.
            // Impede o envio de comandos para um hardware que ainda está em
            // reset.
            if (now - _bootTimestamp > 1000)
            {
                _status = InitStatus::CONNECTING;
                _lastAttemptTime = now;
            }
            break;

        case InitStatus::CONNECTING:
            // Tenta o handshake serial via biblioteca DFRobot a cada 500ms
            if (now - _lastAttemptTime > 500)
            {
                _lastAttemptTime = now;
                if (_df1201s.begin(_serial))
                {
                    // Conexão física estabelecida. Próximo passo: Configuração
                    // de modo.
                    _status = InitStatus::SETTING_MODE;
                    Serial.println(F("[AUDIO] Serial sincronizada. "
                                     "Configurando modo MUSIC..."));

                    // Garante que o player está no modo música e silencia
                    // avisos de voz ("Music")
                    _df1201s.switchFunction(_df1201s.MUSIC);
                    _df1201s.setPrompt(false);

                    // Reinicia o timer para a espera obrigatória pós-troca de
                    // modo (2s)
                    _lastAttemptTime = now;
                }
            }
            break;

        case InitStatus::SETTING_MODE:
            // O DFPlayer Pro necessita de até 2 segundos após a troca de função
            // para processar a leitura do cartão SD/Flash e finalizar sons de
            // prompt.
            if (now - _lastAttemptTime > 2000)
            {
                // Configuração final de reprodução e volume
                _df1201s.setPlayMode(DFRobot_DF1201S::SINGLE);
                _df1201s.setVol(_settings.defaultVolume);
                _currentVolume = _settings.defaultVolume;

                _status = InitStatus::READY;
                Serial.println(
                    F("[AUDIO] Subsistema de áudio totalmente pronto!")
                );

                // Se houve solicitação de áudio durante o período de boot,
                // executa agora
                if (_hasPendingFile)
                {
                    Serial.print(F("[AUDIO] Executando track pendente do boot: "
                    ));
                    Serial.println(_pendingFile);
                    // Usamos o sistema de comando assíncrono aqui também
                    _nextCommand = AudioCommand::PLAY_FILE;
                    _commandArg = _pendingFile;
                    _hasPendingFile = false;
                }
            }
            break;

        default:
            break;
    }
}

/** @section Comandos de Áudio */

/**
 * @brief Toca um arquivo de áudio. Suporta comandos pendentes durante o boot.
 */
void AudioController::playFile(const AudioTrack& track)
{
    // Cancela qualquer sequência ativa e limpa comando de Stop pendente
    _sequenceTracks = nullptr;
    _nextCommand = AudioCommand::NONE;

    if (_status != InitStatus::READY)
    {
        _pendingFile = track.id;
        _hasPendingFile = true;
        return;
    }

    _commandArg = track.id;
    _nextCommand = AudioCommand::PLAY_FILE;
}

/**
 * @brief Reproduz uma sequência de arquivos de áudio em ordem.
 */
void AudioController::playSequence(
    const AudioPlaylist& playlist,
    uint8_t maxLoops
)
{
    if (playlist.count == 0)
        return;

    // Limpa qualquer comando anterior para assumir a nova playlist
    _nextCommand = AudioCommand::NONE;
    _sequenceTracks = playlist.tracks;
    _sequenceCount = playlist.count;
    _sequenceIndex = 0;
    _sequenceMaxLoops = maxLoops;
    _sequenceCurrentLoop = 1;
    _wasPlayingLastCheck = true;

    _commandArg = playlist.tracks[0].id;
    _nextCommand = AudioCommand::PLAY_FILE;
}

/**
 * @brief Toca um arquivo em modo de repetição contínua (loop).
 */
void AudioController::playLoop(const AudioTrack& track)
{
    _sequenceTracks = nullptr;
    _nextCommand = AudioCommand::NONE;
    if (_status != InitStatus::READY)
        return;

    _commandArg = track.id;
    _nextCommand = AudioCommand::PLAY_LOOP;
}

/**
 * @brief Pausa a reprodução e limpa comandos pendentes.
 */
void AudioController::stop()
{
    // Limpa TUDO imediatamente na RAM para que nenhum comando PLAY
    // posterior nesta mesma transição consiga executar.
    _sequenceTracks = nullptr;
    _sequenceCount = 0;
    _sequenceMaxLoops = 1;
    _sequenceCurrentLoop = 1;
    _nextCommand = AudioCommand::STOP; // Sobrescreve qualquer PLAY

    if (_status != InitStatus::READY)
    {
        _hasPendingFile = false;
        return;
    }
}

/**
 * @brief Entra no modo de hibernação, desligando o amplificador.
 */
void AudioController::hibernate()
{
    _sequenceTracks = nullptr;
    if (_status != InitStatus::READY)
        return;
    _nextCommand = AudioCommand::HIBERNATE;
}

/** @section Gestão de Volume */

/**
 * @brief Atualiza o volume do hardware apenas se houver mudança significativa.
 */
void AudioController::setVolume(uint8_t volume)
{
    if (_status != InitStatus::READY)
        return;

    // Clamping de segurança para o protocolo serial do hardware (0-30)
    uint8_t target = (volume > 30) ? 30 : volume;

    if (target != _currentVolume)
    {
        _currentVolume = target;
        _df1201s.setVol(_currentVolume);
    }
}

/**
 * @brief Lê o potenciômetro e aplica filtros de histerese.
 */
void AudioController::processVolumePot()
{
    // Leitura do pino ADC configurado (0-4095)
    int rawValue = analogRead(_potPin);

    // Mapeamento proporcional para a escala do driver (0-30)
    uint8_t mappedVol = (uint8_t)map(rawValue, 0, 4095, 0, 30);

    // Filtro de histerese para evitar flutuações e comandos seriais
    // desnecessários
    if (abs((int)mappedVol - (int)_currentVolume) >= 2)
    {
        setVolume(mappedVol);
    }
}
