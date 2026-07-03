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
 * @brief Central context of the Finite State Machine (FSM) and hardware
 * manager.
 *
 * This class is the "brain" of Wash-Buddy. It coordinates the transitions
 * between the different states of the hand-washing ritual, holds the
 * references to every hardware controller (Display, Motion, RFID, Audio,
 * Power), and applies the global business rules.
 *
 * Among its responsibilities:
 * - Manage the lifecycle of the states (Enter, Update, Exit).
 * - Control the repeat limit of each stage to avoid infinite loops or
 * misuse.
 * - Provide centralized hardware access to the State objects.
 */
class GameController
{
    public:
        /**
         * @brief GameController constructor.
         *
         * Initializes the hardware references and allocates the state pool.
         *
         * @param display Reference to the OLED display orchestrator.
         * @param motion Reference to the servo and animation controller.
         * @param rfid Reference to the RFID tag reader.
         * @param audio Reference to the DFPlayer audio controller.
         * @param power Reference to the main power controller.
         */
        GameController(
            DisplayOrchestrator& display,
            MotionController& motion,
            RFIDReader& rfid,
            AudioController& audio,
            PowerController& power
        );

        /**
         * @brief Destructor that frees the memory allocated for the states.
         *
         * Ensures every instance in the _states array is deleted.
         */
        ~GameController();

        /**
         * @brief Initializes the controller and sets the initial state.
         *
         * Configures the BOOT state as the FSM's entry point.
         */
        void init();

        /**
         * @brief Main processing loop of the controller.
         *
         * Must be called repeatedly from the Arduino main loop(). Updates
         * the current state, processes the debug button, and the behavior
         * engine.
         */
        void update();

        /**
         * @brief Processes an RFID tag read asynchronously.
         *
         * Delegates the handling logic to the current FSM state.
         *
         * @param uid The string containing the UID of the tag that was read.
         */
        void processRFIDTag(const String& uid);

        /**
         * @brief Changes the current FSM state using a direct pointer.
         *
         * @param newState Pointer to the new State instance.
         */
        void changeState(State* newState);

        /**
         * @brief Changes the current FSM state using the enum.
         *
         * Includes the validation logic for the repeats allowed during the
         * ritual.
         *
         * @param stateEnum Identifier of the desired new state.
         */
        void changeState(RobotState stateEnum);

        /**
         * @brief Returns the identifier of the state currently running.
         *
         * @return RobotState Enum of the current state.
         */
        RobotState getCurrentStateEnum() const;

        /**
         * @brief Returns the last ritual state that was executed
         * successfully.
         *
         * @return RobotState The last recorded ritual state.
         */
        RobotState getLastRitualState() const { return _lastRitualState; }

        /**
         * @brief Checks whether a given state is part of the wash ritual.
         *
         * @param state The state to check.
         * @return true If it is a ritual state (WET, SOAP, etc.), false
         * otherwise.
         */
        static bool isRitualState(RobotState state);

        /**
         * @brief Converts a RobotState enum value into a readable string.
         *
         * @param state The state to convert.
         * @return const char* Textual name of the state (e.g. "IDLE").
         */
        static const char* getStateName(RobotState state);

        /**
         * @brief Attempts to repeat the last executed ritual state.
         *
         * Used when a repeat tag is detected.
         */
        void handleRepeat();

        /**
         * @brief Returns the state prior to the last transition.
         *
         * @return State* Pointer to the previous state.
         */
        State* getPreviousState() const { return _previousState; }

        /**
         * @brief Returns the timestamp of when the current state started.
         *
         * @return unsigned long Time in milliseconds (millis).
         */
        unsigned long getStateStartTime() const { return _stateStartTime; }

        /**
         * @brief Provides access to the display orchestrator.
         *
         * @return DisplayOrchestrator& Reference to the display object.
         */
        DisplayOrchestrator& getDisplay() { return _display; }

        /**
         * @brief Provides access to the motion controller.
         *
         * @return MotionController& Reference to the servo controller.
         */
        MotionController& getMotion() { return _motion; }

        /**
         * @brief Provides access to the RFID reader.
         *
         * @return RFIDReader& Reference to the RFID sensor object.
         */
        RFIDReader& getRFID() { return _rfid; }

        /**
         * @brief Provides access to the audio controller.
         *
         * @return AudioController& Reference to the DFPlayer controller.
         */
        AudioController& getAudio() { return _audio; }

        /**
         * @brief Provides access to the power controller.
         *
         * @return PowerController& Reference to the power controller.
         */
        PowerController& getPower() { return _power; }

        /**
         * @brief Provides access to the random behavior engine.
         *
         * @return BehaviorEngine& Reference to the organic animation engine.
         */
        BehaviorEngine& getBehaviors() { return _behaviors; }

        /**
         * @brief Processes the physical debug button logic.
         *
         * Detects short and long presses to toggle debug mode and states.
         */
        void processDebugButton();

        /**
         * @brief Ends all interactions and activities, centers the motors,
         * and calls the system shutdown on the power module.
         */
        void shutdownSystem();

    private:
        DisplayOrchestrator& _display; ///< Visual orchestrator
        MotionController& _motion;     ///< Physical controller (servos)
        RFIDReader& _rfid;             ///< Input sensor (tags)
        AudioController& _audio;       ///< Audio controller
        PowerController& _power;       ///< Main power controller
        BehaviorEngine _behaviors;     ///< Organic life engine

        State* _currentState;  ///< State currently running
        State* _previousState; ///< Previous state for reference
        RobotState _lastRitualState =
            RobotState::BOOT; ///< Memory of the ritual's progress

        unsigned long
            _stateStartTime; ///< Timestamp of the current state's start
        int _repeatCount =
            0; ///< Counter of consecutive attempts on the same state

        bool _isShuttingDown =
            false; ///< Indicates whether the system is shutting down

        // The pool instances now use the dynamic STATE_COUNT
        State* _states[static_cast<int>(RobotState::STATE_COUNT)];

        // Debug control variables
        bool _isDebugMode = false;
        unsigned long _buttonPressTime = 0;
        bool _buttonWasPressed = false;
        bool _debugToggleHandled = false;
        unsigned long _debugTextClearTime =
            0; ///< Timer to clear temporary debug messages from the screen

        /**
         * @brief Allocates and initializes every state instance in the
         * _states array.
         */
        void initializeStates();

        /**
         * @brief Resets the ritual's progress and counters.
         */
        void resetRitualProgress();
};
