#include "PowerController.h"

PowerController::PowerController(uint8_t mosfetPin, uint8_t shutdownPin)
    : _mosfetPin(mosfetPin), _shutdownPin(shutdownPin)
{
}

void PowerController::init()
{
    // Configura os motores (Inicia ligado)
    pinMode(_mosfetPin, OUTPUT);
    enableMotors();

    // Configura o pino de Shutdown (Inicia desligado / LOW)
    pinMode(_shutdownPin, OUTPUT);
    digitalWrite(_shutdownPin, LOW);

    Serial.println(
        F("[POWER] Controlador inicializado. Motores ON, Shutdown LOW.")
    );
}

void PowerController::enableMotors() { digitalWrite(_mosfetPin, HIGH); }

void PowerController::disableMotors()
{
    digitalWrite(_mosfetPin, LOW);
    Serial.println(F("[POWER] Motores DESATIVADOS."));
}

void PowerController::requestSystemShutdown()
{
    Serial.println(F("[POWER] ENVIANDO SINAL DE DESLIGAMENTO (HIGH)..."));

    // 1. Desliga os motores imediatamente para reduzir carga
    disableMotors();

    // 2. Coloca o pino de shutdown em HIGH
    digitalWrite(_shutdownPin, HIGH);

    // 3. Aguarda os 10 segundos necessários para o módulo de bateria atuar.
    // Como o sistema vai cair, podemos usar delay bloqueante aqui.
    Serial.println(F("[POWER] Aguardando 10s para queda de energia..."));
    for (int i = 10; i > 0; i--)
    {
        Serial.print(i);
        Serial.print(F("... "));
        delay(1000);
    }

    // Se chegar aqui, o hardware falhou em cortar a luz.
    Serial.println(F("\n[POWER] ERRO: O sistema nao desligou!"));
}
