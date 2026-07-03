#pragma once
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * @class RFIDReader
 * @brief Simplified interface for the MFRC522 reader.
 *
 * Provides non-blocking methods to detect and identify RFID tags during
 * the wash ritual.
 */
class RFIDReader
{
    public:
        /**
         * @brief Reader constructor.
         * @param sda Select pin (SDA/SS).
         * @param rst Reset pin.
         */
        RFIDReader(uint8_t sda, uint8_t rst);

        /** @brief Initializes the hardware and turns on the antenna. */
        void init();

        /**
         * @brief Checks whether a new tag is nearby.
         * @return True if a new card was detected.
         */
        bool isCardPresent();

        /**
         * @brief Reads the identifier.
         * @return Hexadecimal UID string, or empty on failure.
         */
        String readCardUID();

    private:
        MFRC522 _mfrc522;         ///< Low-level driver
        uint8_t _sdaPin, _rstPin; ///< Physical pins
        unsigned long _lastReadTime = 0;

        static constexpr unsigned long READ_DEBOUNCE_MS = 800;
};
