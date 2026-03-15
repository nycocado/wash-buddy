#pragma once
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * @class RFIDReader
 * @brief Interface simplificada para leitura de tags RFID (MFRC522).
 *
 * Esta classe abstrai a complexidade da biblioteca MFRC522, fornecendo
 * métodos diretos para inicialização, detecção de presença e extração
 * de UIDs formatados.
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

        /**
         * @brief Inicializa o hardware do leitor e ativa a antena.
         */
        void init();

        /**
         * @brief Verifica se há um novo cartão no campo de leitura.
         * @return True se um cartão foi detectado.
         */
        bool isCardPresent();

        /**
         * @brief Lê e formata o UID do cartão atual.
         * @return String com o UID formatado (ex: "AA:BB:CC:DD"), ou vazio se
         * falhar.
         */
        String readCardUID();

        /**
         * @brief Desativa a antena e coloca o leitor em modo de baixo consumo.
         */
        void prepareForSleep();

        /**
         * @brief Reativa o leitor saindo do modo de economia.
         */
        void wakeUp();

    private:
        MFRC522 _mfrc522;
        uint8_t _sdaPin, _rstPin;
        unsigned long _lastReadTime = 0;
        static constexpr unsigned long READ_DEBOUNCE_MS = 800;
};
