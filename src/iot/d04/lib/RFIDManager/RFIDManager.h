#pragma once
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

class RFIDManager
{
    public:
        RFIDManager(uint8_t sda, uint8_t rst);
        void init();
        bool isCardPresent();
        String readCardUID();
        void prepareForSleep();
        void wakeUp();

    private:
        MFRC522 _mfrc522;
        uint8_t _sdaPin, _rstPin;
};
