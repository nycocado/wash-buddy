#pragma once

#include <Arduino.h>

/**
 * @class PowerController
 * @brief Gerencia a alimentação de periféricos e o estado de energia.
 *
 * Responsável por acionar o MOSFET dos motores e sinalizar o desligamento
 * do sistema para preservação da bateria.
 */
class PowerController
{
    public:
        /**
         * @brief Construtor do controlador.
         * @param mosfetPin Pino de ativação dos motores.
         * @param shutdownPin Pino de desligamento do sistema.
         */
        PowerController(uint8_t mosfetPin, uint8_t shutdownPin);

        /** @brief Configura os pinos e ativa os motores por padrão. */
        void init();

        /** @brief Ativa a alimentação dos servomotores. */
        void enableMotors();

        /** @brief Corta a alimentação dos servomotores. */
        void disableMotors();

        /** @brief Sinaliza ao hardware para cortar a energia geral. */
        void requestSystemShutdown();

    private:
        uint8_t _mosfetPin;   ///< Pino de controle do MOSFET
        uint8_t _shutdownPin; ///< Pino de sinal de desligamento
};
