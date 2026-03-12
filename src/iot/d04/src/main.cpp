#include "DisplayManager.h"
#include "MotionManager.h"
#include "RFIDManager.h"
#include "RobotState.h"
#include <Arduino.h>
#include <Wire.h>

constexpr uint8_t SPI_SCK_PIN = 14;
constexpr uint8_t SPI_MISO_PIN = 12;
constexpr uint8_t SPI_MOSI_PIN = 13;
constexpr uint8_t RFID_SDA_PIN = 23;
constexpr uint8_t RFID_RST_PIN = 17;
constexpr uint8_t OLED_SDA_PIN = 21;
constexpr uint8_t OLED_SCL_PIN = 22;
constexpr uint8_t SERVO_ARM_L_PIN = 27;
constexpr uint8_t SERVO_ARM_R_PIN = 26;
constexpr uint8_t SERVO_HEAD_PIN = 25;

RFIDManager rfid(RFID_SDA_PIN, RFID_RST_PIN);
DisplayManager display;
MotionManager motion(SERVO_ARM_L_PIN, SERVO_ARM_R_PIN, SERVO_HEAD_PIN);

RobotState states[] = {
    RobotState::BOOT,
    RobotState::IDLE,
    RobotState::WET,
    RobotState::SOAP,
    RobotState::SCRUB,
    RobotState::RINSE,
    RobotState::DRY,
    RobotState::SUCCESS,
    RobotState::ERROR};
int currentStateIndex = 0;

unsigned long lastChangeTime = 0;
unsigned long lastFrameTime = 0;

void setup()
{
    Serial.begin(115200);

    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
    rfid.init();

    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    display.init();

    motion.init();

    lastFrameTime = millis();
}

void loop()
{
    unsigned long now = millis();
    float deltaTime = (now - lastFrameTime) / 1000.0f;
    lastFrameTime = now;

    display.update();
    motion.update(deltaTime);

    if (now - lastChangeTime > 3000)
    {
        lastChangeTime = now;

        RobotState newState = states[currentStateIndex];

        switch (newState)
        {
            case RobotState::BOOT:
                display.setEyeMood(EyeMood::DEFAULT);
                display.setParticleEffect(EffectType::NONE);
                motion.centerAll();
                break;

            case RobotState::IDLE:
                display.setEyeMood(EyeMood::DEFAULT);
                display.setEyeIdleMode(true);
                display.setParticleEffect(EffectType::NONE);
                motion.lookLeft();
                motion.moveArmL(120);
                break;

            case RobotState::WET:
                display.setEyeMood(EyeMood::TIRED);
                display.setParticleEffect(EffectType::RAIN_LIGHT);
                motion.lookRight();
                motion.moveArmR(60);
                break;

            case RobotState::SOAP:
                display.lookAt(EyePosition::N);
                display.setParticleEffect(EffectType::BUBBLES);
                motion.centerAll();
                break;

            case RobotState::SCRUB:
                display.setEyeMood(EyeMood::HAPPY);
                display.setParticleEffect(EffectType::BUBBLES);
                motion.moveArmL(160, 20.0f);
                motion.moveArmR(20, 20.0f);
                break;

            case RobotState::RINSE:
                display.lookAt(EyePosition::S);
                display.setParticleEffect(EffectType::RAIN_HEAVY);
                motion.centerAll();
                break;

            case RobotState::DRY:
                display.setEyeMood(EyeMood::TIRED);
                display.setParticleEffect(EffectType::WIND);
                motion.lookLeft();
                break;

            case RobotState::SUCCESS:
                display.playHappy();
                display.setParticleEffect(EffectType::CONFETTI);
                motion.waveHands();
                break;

            case RobotState::ERROR:
                display.setEyeMood(EyeMood::ANGRY);
                display.playConfused();
                display.setParticleEffect(EffectType::NONE);
                motion.moveHead(60, 30.0f);
                motion.moveArmL(180, 20.0f);
                break;
        }

        currentStateIndex = (currentStateIndex + 1) % 9;
    }
}
