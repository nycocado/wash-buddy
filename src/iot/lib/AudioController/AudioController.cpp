#include "AudioController.h"

/** @section Lifecycle and Initialization */

/**
 * @brief Constructor for the audio control class.
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
 * @brief Initial hardware and communication setup.
 *
 * Starts the Serial2 port but does not block execution waiting for the
 * hardware.
 */
void AudioController::init()
{
    // Prepares the potentiometer pin for analog reading
    pinMode(_potPin, INPUT);

    // Initializes the HardwareSerial with the DFPlayer Pro's default baud
    // rate (115200)
    _serial.begin(115200, SERIAL_8N1, _rxPin, _txPin);

    // Starts the timer for the asynchronous state machine
    _bootTimestamp = millis();
    _status = InitStatus::WAKING_UP;

    Serial.println(F(
        "[AUDIO] Orquestrador de som carregado e aguardando boot do hardware."
    ));
}

/**
 * @brief Periodic update of the audio subsystem.
 *
 * Manages the resilient initialization state machine and reads the
 * volume potentiometer with cadence control.
 */
void AudioController::update()
{
    // While not ready, exclusively processes the initialization sequence
    if (_status != InitStatus::READY)
    {
        handleInitialization();
        return;
    }

    unsigned long now = millis();

    // Volume processing via the potentiometer, with cadence control
    if (now - _lastUpdate > _settings.updateIntervalMs)
    {
        _lastUpdate = now;
        processVolumePot();
    }

    // --- COMMAND PROCESSING (thread-safe) ---
    // We execute only one hardware command per update cycle to avoid
    // saturating the serial bus.
    if (_nextCommand != AudioCommand::NONE)
    {
        switch (_nextCommand)
        {
            case AudioCommand::PLAY_FILE:
                _df1201s.setPlayMode(DFRobot_DF1201S::SINGLE);
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
        return; // Exits to give the hardware time to process before
                // isPlaying()
    }

    // Audio sequence (playlist) logic
    if (_sequenceTracks != nullptr)
    {
        // We only check isPlaying every 300ms to avoid stalling the
        // serial port
        if (now - _lastSequenceCheck > 300)
        {
            _lastSequenceCheck = now;
            bool isPlayingNow = _df1201s.isPlaying();

            // If it was playing and now isn't, it moved on to the next
            // track
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
                        // Increments the loop counter and restarts the
                        // playlist
                        _sequenceCurrentLoop++;
                        _sequenceIndex = 0;
                        _df1201s.playFileNum(_sequenceTracks[0].id);
                        _wasPlayingLastCheck = true;
                    }
                    else
                    {
                        _sequenceTracks = nullptr; // End of playlist
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
 * @section Resilient Initialization Logic
 * Follows the manufacturer-suggested protocol: power-on -> handshake ->
 * mode switch -> ready.
 */
void AudioController::handleInitialization()
{
    unsigned long now = millis();

    switch (_status)
    {
        case InitStatus::WAKING_UP:
            // Waits 1000ms for the module's full electrical
            // stabilization. Prevents sending commands to hardware that's
            // still resetting.
            if (now - _bootTimestamp > 1000)
            {
                _status = InitStatus::CONNECTING;
                _lastAttemptTime = now;
            }
            break;

        case InitStatus::CONNECTING:
            // Attempts the serial handshake via the DFRobot library every
            // 500ms
            if (now - _lastAttemptTime > 500)
            {
                _lastAttemptTime = now;
                if (_df1201s.begin(_serial))
                {
                    // Physical connection established. Next step: mode
                    // configuration.
                    _status = InitStatus::SETTING_MODE;
                    Serial.println(
                        F("[AUDIO] Serial sincronizada. "
                          "Configurando modo MUSIC...")
                    );

                    // Ensures the player is in music mode and silences
                    // voice prompts ("Music")
                    _df1201s.switchFunction(_df1201s.MUSIC);
                    _df1201s.setPrompt(false);

                    // Restarts the timer for the mandatory post-mode-switch
                    // wait (2s)
                    _lastAttemptTime = now;
                }
            }
            break;

        case InitStatus::SETTING_MODE:
            // The DFPlayer Pro needs up to 2 seconds after the function
            // switch to process reading the SD/Flash card and finish its
            // prompt sounds.
            if (now - _lastAttemptTime > 2000)
            {
                // Final playback and volume configuration
                _df1201s.setPlayMode(DFRobot_DF1201S::SINGLE);
                _df1201s.setVol(_settings.defaultVolume);
                _currentVolume = _settings.defaultVolume;

                _status = InitStatus::READY;
                Serial.println(
                    F("[AUDIO] Subsistema de áudio totalmente pronto!")
                );

                // If an audio request came in during the boot period,
                // executes it now
                if (_hasPendingFile)
                {
                    Serial.print(
                        F("[AUDIO] Executando track pendente do boot: ")
                    );
                    Serial.println(_pendingFile);
                    // We use the asynchronous command system here as well
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

/** @section Audio Commands */

/**
 * @brief Plays an audio file. Supports pending commands during boot.
 */
void AudioController::playFile(const AudioTrack& track)
{
    // Cancels any active sequence and clears a pending stop command
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
 * @brief Plays a sequence of audio files in order.
 */
void AudioController::playSequence(
    const AudioPlaylist& playlist,
    uint8_t maxLoops
)
{
    if (playlist.count == 0)
        return;

    // Clears any previous command to take over the new playlist
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
 * @brief Pauses playback and clears any pending commands.
 */
void AudioController::stop()
{
    // Clears EVERYTHING immediately in RAM so no later PLAY command in
    // this same transition can execute.
    _sequenceTracks = nullptr;
    _sequenceCount = 0;
    _sequenceMaxLoops = 1;
    _sequenceCurrentLoop = 1;
    _nextCommand = AudioCommand::STOP; // Overrides any PLAY

    if (_status != InitStatus::READY)
    {
        _hasPendingFile = false;
        return;
    }
}

/**
 * @brief Enters hibernation mode, turning off the amplifier.
 */
void AudioController::hibernate()
{
    _sequenceTracks = nullptr;
    if (_status != InitStatus::READY)
        return;
    _nextCommand = AudioCommand::HIBERNATE;
}

/** @section Volume Management */

/**
 * @brief Updates the hardware volume only when there's a meaningful
 * change.
 */
void AudioController::setVolume(uint8_t volume)
{
    if (_status != InitStatus::READY)
        return;

    // Safety clamping for the hardware's serial protocol (0-30)
    uint8_t target = (volume > 30) ? 30 : volume;

    if (target != _currentVolume)
    {
        _currentVolume = target;
        _df1201s.setVol(_currentVolume);
    }
}

/**
 * @brief Reads the potentiometer and applies hysteresis filtering.
 */
void AudioController::processVolumePot()
{
    // Reads the configured ADC pin (0-4095)
    int rawValue = analogRead(_potPin);

    // Proportional mapping to the driver's scale (0-30)
    uint8_t mappedVol = (uint8_t)map(rawValue, 0, 4095, 0, 30);

    // Hysteresis filter to avoid fluctuations and unnecessary serial
    // commands
    if (abs((int)mappedVol - (int)_currentVolume) >= 2)
    {
        setVolume(mappedVol);
    }
}
