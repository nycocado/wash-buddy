#pragma once

#include "AudioTypes.h"
#include <Arduino.h>
#include <DFRobot_DF1201S.h>
#include <HardwareSerial.h>

/**
 * @brief Basic hardware and timing settings for the audio subsystem.
 */
struct AudioSettings
{
        uint8_t defaultVolume = 15; ///< Initial volume on power-up (0-30)
        uint32_t updateIntervalMs =
            100; ///< Interval between potentiometer reads

        /**
         * @brief Constructor for the configuration structure.
         * @param vol Desired initial volume.
         * @param interval Time in milliseconds between volume updates.
         */
        AudioSettings(uint8_t vol = 15, uint32_t interval = 100)
            : defaultVolume(vol), updateIntervalMs(interval)
        {
        }
};

/**
 * @class AudioController
 * @brief Resilient interface to control the DFPlayer Pro (DF1201S).
 *
 * This class manages the serial communication with the audio module in a
 * non-blocking way, using an internal state machine to ensure the system
 * stays smooth and the hardware initializes asynchronously.
 */
class AudioController
{
    public:
        /**
         * @enum InitStatus
         * @brief Represents the stages of the hardware's initialization
         * lifecycle.
         */
        enum class InitStatus
        {
            WAKING_UP,    ///< Waiting for the chip's electrical
                          ///< stabilization (1s)
            CONNECTING,   ///< Attempting synchronization via the library's
                          ///< AT commands
            SETTING_MODE, ///< Configuring MUSIC mode and silencing voice
                          ///< prompts
            READY,        ///< Hardware online, configured, and ready to use
            ERROR         ///< Persistent communication failure detected
        };

        /**
         * @brief AudioController constructor via dependency injection.
         *
         * @param serial Reference to the HardwareSerial used (Serial2).
         * @param rxPin ESP32 receive pin connected to the player's TX.
         * @param txPin ESP32 transmit pin connected to the player's RX.
         * @param potPin Analog pin (ADC) for the volume potentiometer.
         * @param settings Structure with the initial audio settings.
         */
        AudioController(
            HardwareSerial& serial,
            uint8_t rxPin,
            uint8_t txPin,
            uint8_t potPin,
            const AudioSettings& settings = AudioSettings()
        );

        /**
         * @brief Initializes the serial port and prepares the boot state.
         * This function does not block execution waiting for the
         * hardware.
         */
        void init();

        /**
         * @brief Processes the state machine and updates the hardware
         * volume. Must be called continuously, ideally from a dedicated
         * task.
         */
        void update();

        /**
         * @brief Plays an audio file via its AudioTrack structure.
         */
        void playFile(const AudioTrack& track);

        /**
         * @brief Plays a playlist (sequence) of audio files.
         * @param maxLoops Maximum number of times the playlist can play (1
         * = plays once, no looping).
         */
        void playSequence(const AudioPlaylist& playlist, uint8_t maxLoops = 1);

        /**
         * @brief Stops the current playback.
         */
        void stop();

        /**
         * @brief Sets the hardware's volume level.
         * @param volume Value between 0 (mute) and 30 (max).
         */
        void setVolume(uint8_t volume);

        /**
         * @brief Enters hibernation mode, turning off the amplifier.
         */
        void hibernate();

    private:
        /**
         * @enum AudioCommand
         * @brief Internal commands to avoid thread conflicts.
         */
        enum class AudioCommand
        {
            NONE,
            PLAY_FILE,
            STOP,
            HIBERNATE
        };

        DFRobot_DF1201S _df1201s; ///< Low-level library instance
        HardwareSerial& _serial;  ///< Serial bus in use
        uint8_t _rxPin;           ///< RX pin physical configuration
        uint8_t _txPin;           ///< TX pin physical configuration
        uint8_t _potPin;          ///< Potentiometer pin physical configuration
        AudioSettings _settings;  ///< Active configuration parameters

        InitStatus _status =
            InitStatus::WAKING_UP; ///< Current initialization state
        uint8_t _currentVolume;    ///< Current volume cache
        unsigned long _lastUpdate; ///< Timestamp of the last ADC read
        unsigned long
            _lastAttemptTime; ///< Timestamp of the last connection attempt
        unsigned long _bootTimestamp; ///< Timestamp of the driver's start

        uint16_t _pendingFile = 0; ///< Stores the track requested during boot
        bool _hasPendingFile =
            false; ///< Whether a command is waiting for readiness

        AudioCommand _nextCommand =
            AudioCommand::NONE;   ///< Next command to execute
        uint16_t _commandArg = 0; ///< Command argument (e.g. index)

        const AudioTrack* _sequenceTracks =
            nullptr;                   ///< Pointer to the current sequence
        uint8_t _sequenceCount = 0;    ///< Sequence size
        uint8_t _sequenceIndex = 0;    ///< Index of the track currently playing
        uint8_t _sequenceMaxLoops = 1; ///< Maximum playlist repetition limit
        uint8_t _sequenceCurrentLoop =
            1; ///< Counter of the playlist's current run
        unsigned long _lastSequenceCheck =
            0; ///< Timer to check isPlaying() without blocking
        bool _wasPlayingLastCheck =
            false; ///< Holds the last state to avoid switching too early

        unsigned long _lastCommandTime =
            0; ///< Prevents overloading the hardware with AT commands

        /**
         * @brief Reads the potentiometer and applies the volume with noise
         * filtering.
         */
        void processVolumePot();

        /**
         * @brief Manages the state transitions for a safe initialization.
         */
        void handleInitialization();
};
