#pragma once
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * @class RFIDReader
 * @brief Interface simplificada para o leitor MFRC522.
 *
 * Oferece métodos não-bloqueantes para detecção e identificação de tags
 * RFID no ritual de lavagem.
 */
class RFIDReader
{
    public:
        /**
         * @brief Construtor do leitor.
         * @param sda Pino de seleção (SDA/SS).
         * @param rst Pino de reset.
         */
        RFIDReader(uint8_t sda, uint8_t rst);

        /** @brief Inicializa o hardware e liga a antena. */
        void init();

        /**
         * @brief Verifica se existe uma nova tag próxima.
         * @return True se um novo cartão foi detectado.
         */
        bool isCardPresent();

        /**
         * @brief Realiza a leitura do identificador.
         * @return String hexadecimal do UID ou vazia em caso de falha.
         */
        String readCardUID();

        /** @brief Desativa a antena e entra em modo economia. */
        void prepareForSleep();

        /** @brief Religando a antena para novas leituras. */
        void wakeUp();

    private:
        MFRC522 _mfrc522;         ///< Driver de baixo nível
        uint8_t _sdaPin, _rstPin; ///< Pinos físicos
        unsigned long _lastReadTime = 0;

        static constexpr unsigned long READ_DEBOUNCE_MS = 800;
};
