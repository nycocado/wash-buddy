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

#include "Constants.h"
#include "DisplayOrchestrator.h"
#include "GameController.h"
#include "MotionController.h"
#include "RFIDReader.h"
#include <Arduino.h>
#include <Wire.h>

// Instâncias Globais de Hardware e Lógica
RFIDReader rfid(Pins::RFID_SDA, Pins::RFID_RST);
DisplayOrchestrator display;
MotionController motion(Pins::SERVO_ARM_L, Pins::SERVO_ARM_R, Pins::SERVO_HEAD);
GameController game(display, motion, rfid);

// Handlers para Multitarefa (FreeRTOS)
TaskHandle_t DisplayTaskHandle = NULL;
TaskHandle_t RFIDTaskHandle = NULL;
QueueHandle_t rfidQueue;
unsigned long lastMotionTime = 0;

/** @section Tarefas */

/**
 * @brief Tarefa dedicada à atualização do Display OLED.
 * Executada no Core 0 para garantir 60 FPS nas animações dos olhos.
 */
void displayTask(void* pvParameters)
{
    for (;;)
    {
        display.update();
        // Pequeno delay para ceder tempo ao Watchdog do FreeRTOS
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Tarefa dedicada à leitura do RFID.
 * Desacopla a leitura SPI do loop principal de cinemática dos motores.
 */
void rfidTask(void* pvParameters)
{
    char uidBuffer[HardwareConfig::RFID_BUFFER_SIZE];
    for (;;)
    {
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
        vTaskDelay(50 / portTICK_PERIOD_MS); // Polling a cada 50ms
    }
}

/** @section Inicialização */

void setup()
{
    Serial.begin(115200);

    // Configuração do pino do botão com pull-down (já que ele liga no 3.3v)
    pinMode(Pins::BUTTON_DEBUG, INPUT_PULLDOWN);

    // Inicialização do barramento SPI e Leitor RFID
    SPI.begin(Pins::SPI_SCK, Pins::SPI_MISO, Pins::SPI_MOSI);
    rfid.init();

    // Inicialização do barramento I2C e Display OLED (800kHz para maior
    // velocidade)
    Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);
    Wire.setClock(800000);
    Wire.setTimeOut(100); // Prevenção de travamento no I2C
    display.init();

    // Inicialização dos controladores de movimento e lógica
    motion.init();
    game.init();

    // Criação da fila do RFID
    rfidQueue =
        xQueueCreate(5, sizeof(char) * HardwareConfig::RFID_BUFFER_SIZE);

    // Criação da tarefa de display fixada no Core 0
    xTaskCreatePinnedToCore(
        displayTask, "DisplayTask", 8192, NULL, 1, &DisplayTaskHandle, 0
    );

    // Criação da tarefa de RFID fixada no Core 1
    xTaskCreatePinnedToCore(
        rfidTask, "RFIDTask", 4096, NULL, 1, &RFIDTaskHandle, 1
    );

    lastMotionTime = millis();
}

/** @section Loop Principal */

/**
 * @brief Loop principal executado no Core 1.
 * Gerencia a lógica do ritual e as animações físicas.
 */
void loop()
{
    unsigned long now = millis();
    float deltaTime = (now - lastMotionTime) / 1000.0f;
    lastMotionTime = now;

    // Atualiza cinemática dos servos
    motion.update(deltaTime);

    // Processa tags RFID lidas pela task
    char uidBuffer[HardwareConfig::RFID_BUFFER_SIZE];
    if (xQueueReceive(rfidQueue, &uidBuffer, 0) == pdTRUE)
    {
        String uid(uidBuffer);
        game.processRFIDTag(uid
        ); // O próprio GameController barra tags em modo Debug
    }

    // Processa lógica da FSM, sensores e botões
    game.update();
}
