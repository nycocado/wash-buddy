#include "PowerController.h"

PowerController::PowerController(uint8_t mosfetPin, uint8_t shutdownPin)
    : _mosfetPin(mosfetPin), _shutdownPin(shutdownPin)
{
}

void PowerController::init()
{
    // Configures the motors (starts enabled)
    pinMode(_mosfetPin, OUTPUT);
    enableMotors();

    // Configures the shutdown pin (starts disabled / LOW)
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

    // 1. Turns off the motors immediately to reduce load
    disableMotors();

    // 2. Sets the shutdown pin to HIGH
    digitalWrite(_shutdownPin, HIGH);

    // 3. Waits the 10 seconds required for the battery module to act.
    // Since the system is about to lose power, a blocking delay is fine
    // here.
    Serial.println(F("[POWER] Aguardando 10s para queda de energia..."));
    for (int i = 10; i > 0; i--)
    {
        Serial.print(i);
        Serial.print(F("... "));
        delay(1000);
    }

    // If execution reaches here, the hardware failed to cut the power.
    Serial.println(F("\n[POWER] ERRO: O sistema nao desligou!"));
}
