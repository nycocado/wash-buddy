#pragma once

#include <Arduino.h>

/**
 * @namespace Pins
 * @brief Physical pin definitions connecting the ESP32 to its peripherals.
 *
 * This namespace groups the constants that define which ESP32 pins are
 * connected to Wash-Buddy's different hardware modules.
 */
namespace Pins
{
    /**
     * @brief Pins for SPI communication with the RFID reader (MFRC522).
     * The RFID reader is used to identify the objects (soap, towel, etc.)
     * via tags.
     */
    constexpr uint8_t SPI_SCK = 26;  ///< Clock pin (SCK)
    constexpr uint8_t SPI_MISO = 33; ///< Master In Slave Out pin (MISO)
    constexpr uint8_t SPI_MOSI = 25; ///< Master Out Slave In pin (MOSI)
    constexpr uint8_t RFID_SDA = 27; ///< Slave select pin (SS/SDA)
    constexpr uint8_t RFID_RST = 14; ///< MFRC522 reset pin

    /**
     * @brief Pins for I2C communication with the OLED display (SH1106).
     * The display is the main visual interface, showing the robot's
     * expressions.
     */
    constexpr uint8_t OLED_SDA = 17; ///< I2C data pin
    constexpr uint8_t OLED_SCL = 22; ///< I2C clock pin

    /**
     * @brief PWM pins to control the servomotors.
     * These servos give life to the robot, moving arms and head to express
     * emotions. Arms and head use ESP32 hardware timers for simultaneous,
     * jitter-free motion.
     */
    constexpr uint8_t SERVO_ARM_L = 21; ///< Left arm: used to wave or gesture
    constexpr uint8_t SERVO_ARM_R = 19; ///< Right arm: used to wave or gesture
    constexpr uint8_t SERVO_HEAD = 18; ///< Head motion: allows looking sideways

    // --- SENSORS AND INPUTS ---
    constexpr uint8_t POT_VOLUME = 34; ///< Analog potentiometer (ADC)

    /**
     * @brief Debug and forced-navigation button.
     * @note Pressing advances states; holding toggles verbose logs
     */
    constexpr uint8_t BUTTON_DEBUG =
        23; ///< Button for forced navigation between states

    /**
     * @brief Control pin for the N-Channel MOSFET.
     * Enables/disables the servomotors' main power supply.
     */
    constexpr uint8_t MOSFET_MOTORS = 13;

    /**
     * @brief Signal pin for the battery module (power-off).
     * When set to HIGH, signals the hardware to cut the power.
     */
    constexpr uint8_t PIN_SHUTDOWN = 12;

    /**
     * @brief Pins for Serial communication with the DFPlayer Pro (DF1201S).
     * @note Using the ESP32's natural TXD2/RXD2 ports (GPIO 17 and 16).
     */
    constexpr uint8_t AUDIO_TX =
        16; ///< ESP32 TX pin (connects to the player's RX)
    constexpr uint8_t AUDIO_RX =
        4; ///< ESP32 RX pin (connects to the player's TX)

} // namespace Pins

/**
 * @namespace RFIDTags
 * @brief Unique identifiers (UIDs) associated with the ritual's physical
 * objects.
 *
 * Each RFID tag is glued onto a real object used by the child during
 * hand-washing.
 */
namespace RFIDTags
{
    static constexpr const char* FAUCET =
        "6C:D3:D6:D4"; ///< Represents the faucet (start/end of rinsing)
    static constexpr const char* SOAP =
        "AC:F6:D8:D4"; ///< Represents the soap bar
    static constexpr const char* SCRUB =
        "1C:D4:D8:D4"; ///< Represents the scrubbing action
    static constexpr const char* TOWEL =
        "9C:ED:D9:D4"; ///< Represents the towel (finishing step)
} // namespace RFIDTags

/**
 * @namespace AudioConfig
 * @brief Calibration parameters for the sound system.
 */
namespace AudioConfig
{
    constexpr uint8_t DEFAULT_VOLUME = 15; ///< Initial volume (0-30)
    constexpr uint32_t UPDATE_INTERVAL_MS =
        300; ///< Potentiometer reading cadence
} // namespace AudioConfig

#include "AudioTracks.h"

/**
 * @namespace GameConfig
 * @brief Logic and timing parameters for the hand-washing ritual.
 *
 * Defines the wait times and duration of each pedagogical stage of the
 * process.
 */
namespace GameConfig
{
    // Default and global timeouts
    constexpr unsigned long STATE_TIMEOUT_DEFAULT =
        15000; ///< General timeout for any state (15s)
    constexpr unsigned long WAITING_TIMEOUT_MS =
        45000; ///< Maximum time the robot waits for the next action (45s)
    constexpr unsigned long WAITING_LOOK_AROUND_MS =
        5000; ///< Time until the robot starts looking around while waiting
              ///< (5s)
    constexpr unsigned long WAITING_SAD_PHASE_MS =
        15000; ///< Time until the robot switches to a sad mood while
               ///< waiting (15s)
    constexpr unsigned long WAITING_REMINDER_INTERVAL_MS =
        15000; ///< Interval between visual reminders of the next stage (15s)
    constexpr unsigned long ERROR_DISPLAY_MS =
        4000; ///< Duration of the error/attention screen (4s)
    constexpr unsigned long BOOT_DELAY_MS =
        8000; ///< 3.5s (HW boot) + 3.6s (audio) + margin

    // Stage-specific timeouts within the ritual (pedagogical - decided by
    // the educator)
    /**
     * @brief Time allowed for the wetting-hands stage.
     * A quick preparation before applying soap.
     */
    constexpr unsigned long WET_TIMEOUT = 12000;

    /**
     * @brief Time allowed for applying soap.
     * A period meant to ensure the child grabbed enough soap.
     */
    constexpr unsigned long SOAP_TIMEOUT = 12000;

    /**
     * @brief Time allowed for the hand-scrubbing stage.
     * Based on the WHO/CDC recommendation of at least 20 seconds to
     * eliminate pathogens.
     */
    constexpr unsigned long SCRUB_TIMEOUT = 25000;

    /**
     * @brief Time allowed for the final rinse.
     * Ensures all soap and dirt were removed.
     */
    constexpr unsigned long RINSE_TIMEOUT = 20000;

    /**
     * @brief Time allowed for drying the hands.
     * An important final stage to avoid bacteria proliferating on damp
     * hands.
     */
    constexpr unsigned long DRY_TIMEOUT = 25000;

    /**
     * @brief Duration of the final celebration.
     * Extended time for the child to celebrate the victory with the robot.
     */
    constexpr unsigned long SUCCESS_DISPLAY_MS = 15000;

    /**
     * @brief Time the button must be held to enter/exit debug mode.
     */
    constexpr unsigned long DEBUG_LONG_PRESS_MS = 1500;

    /**
     * @brief Minimum press duration to be considered a valid click
     * (debounce). Prevents electrical noise from triggering multiple
     * clicks.
     */
    constexpr unsigned long DEBUG_DEBOUNCE_MS = 50;

    /**
     * @brief Time holding the button before showing the "HOLD..." visual
     * feedback.
     */
    constexpr unsigned long DEBUG_HOLD_FEEDBACK_MS = 500;

    /**
     * @brief Time the "DEBUG OFF" message stays on screen before it
     * disappears.
     */
    constexpr unsigned long DEBUG_MSG_DURATION_MS = 2000;

    // --- BEHAVIOR (IDLE & BEHAVIOR) ---
    constexpr unsigned long IDLE_TIMEOUT_MS =
        120000; ///< Maximum time in the idle state before sleeping (2 minutes)
    constexpr unsigned long IDLE_MIN_PAUSE_MS =
        8000; ///< Minimum pause between actions
    constexpr unsigned long IDLE_MAX_PAUSE_MS =
        15000; ///< Maximum pause between actions
    constexpr unsigned long IDLE_REMINDER_INTERVAL_MS =
        30000; ///< Interval between icons
    constexpr unsigned long IDLE_INITIAL_DELAY_MS =
        5000; ///< Initial delay when entering Idle
    constexpr unsigned long INSTRUCTION_DISPLAY_MS =
        5000; ///< Time the icon stays on screen

} // namespace GameConfig

/**
 * @namespace HardwareConfig
 * @brief General hardware and memory settings.
 */
namespace HardwareConfig
{
    /**
     * @brief Maximum size (in bytes) of the buffer used for RFID UID reads
     * and queues.
     */
    constexpr uint8_t RFID_BUFFER_SIZE = 32;
} // namespace HardwareConfig
