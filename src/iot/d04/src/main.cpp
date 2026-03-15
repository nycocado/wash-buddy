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
unsigned long lastMotionTime = 0;

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

void setup()
{
    Serial.begin(115200);

    // Inicialização do barramento SPI e Leitor RFID
    SPI.begin(Pins::SPI_SCK, Pins::SPI_MISO, Pins::SPI_MOSI);
    rfid.init();

    // Inicialização do barramento I2C e Display OLED (800kHz para maior
    // velocidade)
    Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);
    Wire.setClock(800000);
    display.init();

    // Inicialização dos controladores de movimento e lógica
    motion.init();
    game.init();

    // Criação da tarefa de display fixada no Core 0
    xTaskCreatePinnedToCore(
        displayTask, "DisplayTask", 8192, NULL, 1, &DisplayTaskHandle, 0
    );

    lastMotionTime = millis();
}

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

    // Processa lógica da FSM e sensores
    game.update();
}
