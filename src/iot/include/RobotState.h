#pragma once

/**
 * @enum RobotState
 * @brief Possible states of the Wash-Buddy Finite State Machine (FSM).
 *
 * This enum centralizes every logical behavior of the robot, distinguishing
 * between technical states (BOOT, IDLE, ERROR), wash ritual states (WET,
 * SOAP, SCRUB, RINSE, DRY), and interaction states (WAITING, SUCCESS).
 */
enum class RobotState
{
    BOOT,       ///< Initial hardware and sensor calibration state.
    IDLE,       ///< Resting/waiting state for an RFID tag (faucet/soap).
    WET,        ///< Ritual: wetting the hands (light rain feedback).
    SOAP,       ///< Ritual: applying soap (bubbles feedback).
    SCRUB,      ///< Ritual: scrubbing the hands (bubbles + arm motion
                ///< feedback).
    RINSE,      ///< Ritual: rinsing the hands (heavy rain feedback).
    DRY,        ///< Ritual: drying the hands (wind feedback).
    SUCCESS,    ///< Celebration of the ritual's completion (confetti + wave
                ///< feedback).
    ERROR,      ///< Indication of an incorrect action or out-of-order tag.
    WAITING,    ///< Transition state waiting for the user's next action.
    STATE_COUNT ///< Automatic marker for the total number of states.
};
