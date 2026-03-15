#include "states/WaitingState.h"
#include "ChoreographyLibrary.h"
#include "GameController.h"

void WaitingState::enter(GameController* controller)
{
    // Começa com expressão de preocupação leve
    controller->getDisplay().setEyeMood(eEmotions::Worried);
    _nextActionTime = millis() + 1000;
    _isSadPhase = false;
}

void WaitingState::update(GameController* controller)
{
    MotionController& motion = controller->getMotion();
    DisplayOrchestrator& display = controller->getDisplay();
    unsigned long elapsed = millis() - controller->getStateStartTime();

    // Mantém movimento sutil de respiração nos braços se não houver animação
    // ativa
    if (!motion.isArmLAnimActive() && !motion.isArmRAnimActive())
    {
        float breatheOffset = (sin(millis() / 400.0f) + 1.0f) * 4.0f;
        motion.moveArmL(breatheOffset, 1.0f);
        motion.moveArmR(breatheOffset, 1.0f);
    }

    // Timeout: Se passar do tempo limite (15s), volta para o IDLE (ritual
    // abandonado)
    if (elapsed > GameConfig::WAITING_TIMEOUT_MS)
    {
        controller->changeState(RobotState::IDLE);
        return;
    }

    // Lógica de evolução da impaciência
    if (millis() > _nextActionTime)
    {
        // Se demorar mais de 7 segundos, entra na fase triste (Sad)
        if (!_isSadPhase && elapsed > 7000)
        {
            _isSadPhase = true;
            display.setEyeMood(eEmotions::Sad);
            display.lookAt(0.0f, -0.5f); // Olha para baixo tristemente
            motion.stopAllAnimations();
            _nextActionTime = millis() + 1000;
        }
        else if (!_isSadPhase) // Fase de preocupação (procura pela criança)
        {
            float eyeX = random(-70, 71) / 100.0f;
            display.lookAt(eyeX, 0.0f);
            int headAngle = (eyeX > 0) ? 120 : 60;
            motion.playHeadChoreography(
                ChoreographyLibrary::lookSide(headAngle), 0.0f, false
            );
            _nextActionTime = millis() + random(2000, 4000);
        }
        else // Fase triste (pede ajuda/continuação)
        {
            motion.playArmLChoreography(
                ChoreographyLibrary::askingForHelp(), 0.0f, false
            );
            _nextActionTime = millis() + 4000;
        }
    }
}

void WaitingState::exit(GameController* controller) {}

void WaitingState::handleRFID(GameController* controller, const String& uid)
{
    // Recupera onde o ritual parou para saber qual a próxima tag válida
    RobotState lastRitual = controller->getLastRitualState();

    // Segurança: se não houve ritual ainda, volta para o IDLE
    if (lastRitual == RobotState::BOOT || lastRitual == RobotState::IDLE)
    {
        controller->changeState(RobotState::IDLE);
        return;
    }

    // Lógica de transição baseada na última etapa concluída
    if (lastRitual == RobotState::WET) // Parou em: Molhar as mãos
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::WET); // Repetir
        else if (uid == RFIDTags::SOAP)
            controller->changeState(RobotState::SOAP); // Avançar para Sabão
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SOAP) // Parou em: Passar sabão
    {
        if (uid == RFIDTags::SOAP)
            controller->changeState(RobotState::SOAP); // Repetir
        else if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB); // Avançar para Esfregar
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::SCRUB) // Parou em: Esfregar
    {
        if (uid == RFIDTags::SCRUB)
            controller->changeState(RobotState::SCRUB); // Repetir
        else if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE); // Avançar para Enxágue
        else
            controller->changeState(RobotState::ERROR);
    }
    else if (lastRitual == RobotState::RINSE) // Parou em: Enxágue
    {
        if (uid == RFIDTags::FAUCET)
            controller->changeState(RobotState::RINSE); // Repetir
        else if (uid == RFIDTags::TOWEL)
            controller->changeState(RobotState::DRY); // Avançar para Secar
        else
            controller->changeState(RobotState::ERROR);
    }
    else
    {
        controller->changeState(RobotState::ERROR);
    }
}
