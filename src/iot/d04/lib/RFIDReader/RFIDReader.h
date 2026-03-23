#pragma once
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * @class RFIDReader
 * @brief Interface de alto nível para o leitor de cartões RFID (MFRC522).
 *
 * Esta classe encapsula a biblioteca MFRC522, oferecendo uma interface
 * simplificada e não-bloqueante para detectar e identificar tags RFID
 * utilizadas no ritual de lavagem (ex: sabonete, toalha).
 */
class RFIDReader
{
    public:
        /**
         * @brief Construtor do leitor RFID.
         * @param sda Pino de seleção do chip (SDA/SS).
         * @param rst Pino de reset do hardware.
         */
        RFIDReader(uint8_t sda, uint8_t rst);

        /**
         * @brief Inicializa o hardware do leitor e liga a antena.
         * Deve ser chamado uma única vez no setup() do Arduino.
         */
        void init();

        /**
         * @brief Verifica se existe uma nova tag RFID próxima à antena.
         * Implementa um debounce temporal para evitar leituras repetitivas.
         * @return True se um novo cartão foi detectado com sucesso.
         */
        bool isCardPresent();

        /**
         * @brief Realiza a leitura e formatação do UID da tag.
         * @return String contendo o UID em formato hexadecimal (ex:
         * "A1:B2:C3:D4"). Retorna uma string vazia se a leitura falhar.
         */
        String readCardUID();

        /**
         * @brief Coloca o hardware do MFRC522 em modo de economia de energia.
         * Desativa a antena e o oscilador interno.
         */
        void prepareForSleep();

        /**
         * @brief Acorda o hardware do leitor do modo sleep.
         * Religando a antena e preparando para novas leituras.
         */
        void wakeUp();

    private:
        MFRC522 _mfrc522;
        uint8_t _sdaPin, _rstPin;
        unsigned long _lastReadTime = 0;

        /// Intervalo mínimo entre leituras do mesmo cartão para evitar spam
        static constexpr unsigned long READ_DEBOUNCE_MS = 800;
};
