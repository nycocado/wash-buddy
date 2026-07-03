#pragma once

#include <Arduino.h>

/**
 * @class PowerController
 * @brief Manages peripheral power and the energy state.
 *
 * Responsible for driving the motors' MOSFET and signaling the system
 * shutdown to preserve the battery.
 */
class PowerController
{
    public:
        /**
         * @brief Controller constructor.
         * @param mosfetPin Pin used to enable the motors.
         * @param shutdownPin Pin used to shut down the system.
         */
        PowerController(uint8_t mosfetPin, uint8_t shutdownPin);

        /** @brief Configures the pins and enables the motors by default. */
        void init();

        /** @brief Enables the servomotors' power supply. */
        void enableMotors();

        /** @brief Cuts the servomotors' power supply. */
        void disableMotors();

        /** @brief Signals the hardware to cut the main power. */
        void requestSystemShutdown();

    private:
        uint8_t _mosfetPin;   ///< MOSFET control pin
        uint8_t _shutdownPin; ///< Shutdown signal pin
};
