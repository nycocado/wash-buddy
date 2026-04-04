/**
 * @file main.cpp
 * @brief Ponto de entrada do firmware Wash-Buddy.
 *
 * Este projeto utiliza a arquitetura Dual-Core do ESP32 via FreeRTOS para
 * garantir fluidez na interface de usuário e precisão na lógica de controle.
 *
 * Divisão de Tarefas:
 * - Core 0 (Pro): Dedicado exclusivamente ao DisplayTask. Responsável pelas
 * animações complexas dos olhos e renderização do display OLED. Isso evita que
 * processamentos pesados de lógica ou leitura de sensores causem "stuttering"
 * visual.
 * - Core 1 (App): Executa o loop() principal do Arduino. Gerencia a Máquina de
 * Estados (FSM), leitura do RFID (SPI) e controle dos servomotores (PWM).
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

/** @section Instâncias de Hardware e Lógica */

/// Leitor de tags RFID via barramento SPI
RFIDReader rfid(Pins::RFID_SDA, Pins::RFID_RST);

/// Orquestrador de interface visual para o display OLED
DisplayOrchestrator display;

/// Controlador de cinemática para os servos dos braços e cabeça
MotionController motion(Pins::SERVO_ARM_L, Pins::SERVO_ARM_R, Pins::SERVO_HEAD);

/// Controlador resiliente para o subsistema de áudio DFPlayer Pro
AudioController audio(
    Serial2,
    Pins::AUDIO_RX,
    Pins::AUDIO_TX,
    Pins::POT_VOLUME,
    AudioSettings{AudioConfig::DEFAULT_VOLUME, AudioConfig::UPDATE_INTERVAL_MS}
);

/// Controlador de energia (MOSFET e sinal de desligamento)
PowerController power(Pins::MOSFET_MOTORS, Pins::PIN_SHUTDOWN);

/// Cérebro do sistema: gerencia a FSM e coordena os periféricos
GameController game(display, motion, rfid, audio, power);

/** @section Handlers FreeRTOS */

TaskHandle_t DisplayTaskHandle = NULL; ///< Handler para a tarefa de UI
TaskHandle_t RFIDTaskHandle = NULL;    ///< Handler para a tarefa de sensores
TaskHandle_t AudioTaskHandle = NULL;   ///< Handler para a tarefa de som
QueueHandle_t rfidQueue;          ///< Fila de comunicação para UIDs de tags
unsigned long lastMotionTime = 0; ///< Marca temporal para cálculo de delta-time
bool isSystemSleeping =
    false; ///< Flag para suspender tarefas antes do desligamento

/** @section Tarefas (Tasks) */

/**
 * @brief Tarefa dedicada ao processamento de áudio no Core 1.
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
 * @brief Tarefa dedicada à atualização do Display OLED no Core 0.
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
 * @brief Tarefa dedicada à leitura do RFID no Core 1.
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

/** @section Ciclo de Vida: Inicialização */

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\n[SYSTEM] Wash-Buddy v2.0 - Iniciando..."));

    // Configuração de GPIOs básicos
    pinMode(Pins::BUTTON_DEBUG, INPUT_PULLDOWN);

    // Inicialização do barramento SPI e RFID (Core 1)
    SPI.begin(Pins::SPI_SCK, Pins::SPI_MISO, Pins::SPI_MOSI);
    rfid.init();

    // Inicialização do barramento I2C e Display (Core 0)
    Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);
    Wire.setClock(800000);
    display.init();

    // Inicialização dos controladores
    power.init();
    motion.init();
    audio.init();
    game.init();

    rfidQueue =
        xQueueCreate(5, sizeof(char) * HardwareConfig::RFID_BUFFER_SIZE);

    // --- ORQUESTRAÇÃO DE NÚCLEOS (TASKS) ---

    // Criação da tarefa de display fixada no Core 0
    xTaskCreatePinnedToCore(
        displayTask, "DisplayTask", 8192, NULL, 2, &DisplayTaskHandle, 0
    );

    // Criação da tarefa de áudio fixada no Core 1
    xTaskCreatePinnedToCore(
        audioTask, "AudioTask", 4096, NULL, 1, &AudioTaskHandle, 1
    );

    // Criação da tarefa de RFID fixada no Core 1
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

    // Atualiza cinemática dos servos (Core 1)
    motion.update(deltaTime);

    // Processa tags RFID vindas da fila
    char uidBuffer[HardwareConfig::RFID_BUFFER_SIZE];
    if (xQueueReceive(rfidQueue, &uidBuffer, 0) == pdTRUE)
    {
        game.processRFIDTag(String(uidBuffer));
    }

    // Processa FSM
    game.update();
}
