#include "RFIDReader.h"

/**
 * @section Inicialização e Configuração
 */

RFIDReader::RFIDReader(uint8_t sda, uint8_t rst)
    : _mfrc522(sda, rst), _sdaPin(sda), _rstPin(rst)
{
}

void RFIDReader::init()
{
    _mfrc522.PCD_Init();
    _mfrc522.PCD_AntennaOn();
    Serial.println(F("[RFID] Pronto. Aguardando cartao..."));
}

/**
 * @section Operação de Leitura
 */

bool RFIDReader::isCardPresent()
{
    if (millis() - _lastReadTime < READ_DEBOUNCE_MS)
        return false;
    return _mfrc522.PICC_IsNewCardPresent();
}

String RFIDReader::readCardUID()
{
    if (!_mfrc522.PICC_ReadCardSerial())
        return "";

    _lastReadTime = millis();
    String uid = "";
    if (_mfrc522.uid.size == 0)
        return "";

    for (byte i = 0; i < _mfrc522.uid.size; i++)
    {
        if (i > 0)
            uid += ":";
        if (_mfrc522.uid.uidByte[i] < 0x10)
            uid += "0";
        uid += String(_mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    _mfrc522.PICC_HaltA();
    _mfrc522.PCD_StopCrypto1();
    return uid;
}

/**
 * @section Gerenciamento de Energia
 */

void RFIDReader::prepareForSleep()
{
    _mfrc522.PCD_AntennaOff();
    _mfrc522.PCD_SoftPowerDown();
    Serial.println(F("[RFID] Em sleep."));
}

void RFIDReader::wakeUp()
{
    _mfrc522.PCD_SoftPowerUp();
    _mfrc522.PCD_AntennaOn();
    Serial.println(F("[RFID] Acordado."));
}
