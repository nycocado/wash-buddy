/**
 * @file main.cpp
 * @brief Entry point of the Wash-Buddy firmware.
 *
 * This project uses the ESP32's dual-core architecture via FreeRTOS to
 * ensure a smooth user interface and precise control logic.
 *
 * Task split:
 * - Core 0 (Pro): dedicated exclusively to DisplayTask. Responsible for
 * the complex eye animations and OLED display rendering. This prevents
 * heavy logic processing or sensor reads from causing visual "stuttering".
 * - Core 1 (App): runs the Arduino main loop(). Manages the state machine
 * (FSM), RFID reading (SPI), and servomotor control (PWM).
 */

#include "AudioController.h"
#include "Constants.h"
#include "DisplayOrchestrator.h"
#include "GameController.h"
#include "MotionController.h"
#include "PowerController.h"
#include "RFIDReader.h"
#include <Arduino.h>
#include <Wire.h>

/** @section Hardware and Logic Instances */

/// RFID tag reader over the SPI bus
RFIDReader rfid(Pins::RFID_SDA, Pins::RFID_RST);

/// Visual interface orchestrator for the OLED display
DisplayOrchestrator display;

/// Kinematics controller for the arm and head servos
MotionController motion(Pins::SERVO_ARM_L, Pins::SERVO_ARM_R, Pins::SERVO_HEAD);

/// Resilient controller for the DFPlayer Pro audio subsystem
AudioController audio(
    Serial2,
    Pins::AUDIO_RX,
    Pins::AUDIO_TX,
    Pins::POT_VOLUME,
    AudioSettings{AudioConfig::DEFAULT_VOLUME, AudioConfig::UPDATE_INTERVAL_MS}
);

/// Power controller (MOSFET and shutdown signal)
PowerController power(Pins::MOSFET_MOTORS, Pins::PIN_SHUTDOWN);

/// System's brain: manages the FSM and coordinates the peripherals
GameController game(display, motion, rfid, audio, power);

/** @section FreeRTOS Handlers */

TaskHandle_t DisplayTaskHandle = NULL; ///< Handler for the UI task
TaskHandle_t RFIDTaskHandle = NULL;    ///< Handler for the sensor task
TaskHandle_t AudioTaskHandle = NULL;   ///< Handler for the sound task
QueueHandle_t rfidQueue;               ///< Communication queue for tag UIDs
unsigned long lastMotionTime = 0;      ///< Timestamp for delta-time computation
bool isSystemSleeping = false; ///< Flag to suspend tasks before shutdown

/** @section Tasks */

/**
 * @brief Task dedicated to audio processing on Core 1.
 */
void audioTask(void* pvParameters)
{
    for (;;)
    {
        if (isSystemSleeping)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }
        audio.update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Task dedicated to updating the OLED display on Core 0.
 */
void displayTask(void* pvParameters)
{
    unsigned long lastDisplayTime = millis();
    for (;;)
    {
        if (isSystemSleeping)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }
        unsigned long now = millis();
        float deltaTime = (now - lastDisplayTime) / 1000.0f;
        lastDisplayTime = now;

        display.update(deltaTime);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Task dedicated to RFID reading on Core 1.
 */
void rfidTask(void* pvParameters)
{
    char uidBuffer[HardwareConfig::RFID_BUFFER_SIZE];
    for (;;)
    {
        if (isSystemSleeping)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }
        if (rfid.isCardPresent())
        {
            String uid = rfid.readCardUID();
            if (uid.length() > 0)
            {
                strncpy(uidBuffer, uid.c_str(), sizeof(uidBuffer) - 1);
                uidBuffer[sizeof(uidBuffer) - 1] = '\0';
                xQueueSend(rfidQueue, &uidBuffer, 0);
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/** @section Lifecycle: Initialization */

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\n[SYSTEM] Wash-Buddy v2.0 - Iniciando..."));

    // Basic GPIO configuration
    pinMode(Pins::BUTTON_DEBUG, INPUT_PULLDOWN);

    // SPI bus and RFID initialization (Core 1)
    SPI.begin(Pins::SPI_SCK, Pins::SPI_MISO, Pins::SPI_MOSI);
    rfid.init();

    // I2C bus and display initialization (Core 0)
    Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);
    Wire.setClock(800000);
    display.init();

    // Controller initialization
    power.init();
    motion.init();
    audio.init();
    game.init();

    rfidQueue =
        xQueueCreate(5, sizeof(char) * HardwareConfig::RFID_BUFFER_SIZE);

    // --- CORE ORCHESTRATION (TASKS) ---

    // Creates the display task pinned to Core 0
    xTaskCreatePinnedToCore(
        displayTask, "DisplayTask", 8192, NULL, 2, &DisplayTaskHandle, 0
    );

    // Creates the audio task pinned to Core 1
    xTaskCreatePinnedToCore(
        audioTask, "AudioTask", 4096, NULL, 1, &AudioTaskHandle, 1
    );

    // Creates the RFID task pinned to Core 1
    xTaskCreatePinnedToCore(
        rfidTask, "RFIDTask", 4096, NULL, 1, &RFIDTaskHandle, 1
    );

    lastMotionTime = millis();
}

void loop()
{
    unsigned long now = millis();
    float deltaTime = (now - lastMotionTime) / 1000.0f;
    lastMotionTime = now;

    // Updates the servo kinematics (Core 1)
    motion.update(deltaTime);

    // Processes RFID tags coming from the queue
    char uidBuffer[HardwareConfig::RFID_BUFFER_SIZE];
    if (xQueueReceive(rfidQueue, &uidBuffer, 0) == pdTRUE)
    {
        game.processRFIDTag(String(uidBuffer));
    }

    // Processes the FSM
    game.update();
}
