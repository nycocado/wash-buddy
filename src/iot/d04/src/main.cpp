#include "DisplayManager.h"
#include "GameController.h"
#include "MotionManager.h"
#include "RFIDManager.h"
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

GameController game(display, motion, rfid);

unsigned long lastFrameTime = 0;

void setup()
{
    Serial.begin(115200);

    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
    rfid.init();

    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    display.init();

    motion.init();

    game.init();

    lastFrameTime = millis();
}

void loop()
{
    unsigned long now = millis();
    float deltaTime = (now - lastFrameTime) / 1000.0f;
    lastFrameTime = now;
    display.update();
    motion.update(deltaTime);
    game.update();
}
