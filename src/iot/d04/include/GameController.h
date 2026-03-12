#pragma once

#include "RobotState.h"
#include <Arduino.h>
#include "DisplayManager.h"
#include "MotionManager.h"
#include "RFIDManager.h"

class GameController
{
    public:
        GameController(
            DisplayManager& display,
            MotionManager& motion,
            RFIDManager& rfid
        );

        // Inicialização da lógica (ex: setar estado inicial)
        void init();

        // O coração do jogo: deve ser chamado repetidamente no loop()
        void update();

        // Força uma mudança de estado (útil para interrupções ou debug)
        void changeState(RobotState newState);

        RobotState getCurrentState() const { return _currentState; }

    private:
        DisplayManager& _display;
        MotionManager& _motion;
        RFIDManager& _rfid;

        RobotState _currentState;
        unsigned long _stateStartTime; // Regista quando o estado atual começou

        void handleIdleState();
        void handleWashingState(); // WIP: Pode dividir isto em WET, SOAP, etc.

        // Mapeia UID do cartão para uma ação ou estado
        void processRFIDTag(const String& uid);
};