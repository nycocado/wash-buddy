#pragma once

#include <Arduino.h>

/**
 * @namespace Pins
 * @brief Definições físicas dos pinos de conexão do ESP32 aos periféricos.
 *
 * Este namespace agrupa as constantes que definem quais pinos do ESP32 estão
 * conectados aos diferentes módulos de hardware do Wash-Buddy.
 */
namespace Pins
{
    /**
     * @brief Pinos para comunicação SPI com o Leitor RFID (MFRC522).
     * O leitor RFID é usado para identificar os objetos (sabão, toalha, etc.)
     * via tags.
     */
    constexpr uint8_t SPI_SCK = 33;  ///< Pino de Clock (SCK)
    constexpr uint8_t SPI_MISO = 26; ///< Pino Master In Slave Out (MISO)
    constexpr uint8_t SPI_MOSI = 25; ///< Pino Master Out Slave In (MOSI)
    constexpr uint8_t RFID_SDA = 32; ///< Pino de Seleção de Escravo (SS/SDA)
    constexpr uint8_t RFID_RST = 27; ///< Pino de Reset do MFRC522

    /**
     * @brief Pinos para comunicação I2C com o Display OLED (SH1106).
     * O display é a principal interface visual, mostrando as expressões do
     * robô.
     */
    constexpr uint8_t OLED_SDA = 17; ///< Pino de Dados I2C
    constexpr uint8_t OLED_SCL = 16; ///< Pino de Clock I2C

    /**
     * @brief Pinos PWM para controle dos Servomotores.
     * Estes servos dão vida ao robô, movendo braços e cabeça para expressar
     * emoções. Braços e cabeça utilizam timers de hardware do ESP32 para
     * movimentos simultâneos e fluidos sem jitter.
     */
    constexpr uint8_t SERVO_ARM_L =
        13; ///< Braço Esquerdo: Usado para acenar ou gesticular
    constexpr uint8_t SERVO_ARM_R =
        14; ///< Braço Direito: Usado para acenar ou gesticular
    constexpr uint8_t SERVO_HEAD =
        4; ///< Movimento da Cabeça: Permite olhar para os lados

    // --- SENSORES E ENTRADAS ---
    constexpr uint8_t POT_VOLUME = 34; ///< Potenciômetro analógico (ADC)

    /**
     * @brief Botão de depuração e navegação forçada.
     * @note Pressionar avança estados; Segurar ativa/desativa logs detalhados
     */
    constexpr uint8_t BUTTON_DEBUG =
        23; ///< Botão para navegação forçada entre estados

    /**
     * @brief Pino de controle do MOSFET N-Channel.
     * Habilita/Desabilita a alimentação principal dos Servomotores.
     */
    constexpr uint8_t MOSFET_MOTORS = 22;

    /**
     * @brief Pino de sinal para o módulo de bateria (Power-Off).
     * Quando colocado em HIGH, sinaliza para o hardware cortar a energia.
     */
    constexpr uint8_t PIN_SHUTDOWN = 12;

    /**
     * @brief Pinos para comunicação Serial com o DFPlayer Pro (DF1201S).
     * @note Usando as portas TXD2/RXD2 naturais do ESP32 (GPIO 17 e 16).
     */
    constexpr uint8_t AUDIO_TX =
        19; ///< Pino TX do ESP32 (conecta no RX do player)
    constexpr uint8_t AUDIO_RX =
        21; ///< Pino RX do ESP32 (conecta no TX do player)

} // namespace Pins

/**
 * @namespace RFIDTags
 * @brief Identificadores únicos (UIDs) associados aos objetos físicos do
 * ritual.
 *
 * Cada tag RFID está colada em um objeto real usado pela criança durante a
 * lavagem das mãos.
 */
namespace RFIDTags
{
    static constexpr const char* FAUCET =
        "6C:D3:D6:D4"; ///< Representa a Torneira (início/fim do enxágue)
    static constexpr const char* SOAP =
        "AC:F6:D8:D4"; ///< Representa o Sabonete
    static constexpr const char* SCRUB =
        "1C:D4:D8:D4"; ///< Representa a ação de esfregar as mãos
    static constexpr const char* TOWEL =
        "9C:ED:D9:D4"; ///< Representa a Toalha (finalização)
} // namespace RFIDTags

/**
 * @namespace AudioConfig
 * @brief Parâmetros de calibração do sistema de som.
 */
namespace AudioConfig
{
    constexpr uint8_t DEFAULT_VOLUME = 15; ///< Volume inicial (0-30)
    constexpr uint32_t UPDATE_INTERVAL_MS =
        300; ///< Cadência de leitura do potenciômetro
} // namespace AudioConfig

#include "AudioTracks.h"

/**
 * @namespace GameConfig
 * @brief Parâmetros lógicos e de temporização do ritual de lavagem de mãos.
 *
 * Define os tempos de espera e duração de cada etapa pedagógica do processo.
 */
namespace GameConfig
{
    // Timeouts padrão e globais
    constexpr unsigned long STATE_TIMEOUT_DEFAULT =
        15000; ///< Tempo limite geral para qualquer estado (15s)
    constexpr unsigned long WAITING_TIMEOUT_MS =
        45000; ///< Tempo máximo que o robô espera pela próxima ação (45s)
    constexpr unsigned long WAITING_LOOK_AROUND_MS =
        5000; ///< Tempo até o robô começar a procurar com o olhar na espera
              ///< (5s)
    constexpr unsigned long WAITING_SAD_PHASE_MS =
        15000; ///< Tempo até o robô mudar para humor triste na espera (15s)
    constexpr unsigned long WAITING_REMINDER_INTERVAL_MS =
        15000; ///< Intervalo entre lembretes visuais da próxima etapa (15s)
    constexpr unsigned long ERROR_DISPLAY_MS =
        4000; ///< Duração da tela de erro/atenção (4s)
    constexpr unsigned long BOOT_DELAY_MS =
        8000; ///< 3.5s (Boot HW) + 3.6s (Áudio) + margem

    // Timeouts específicos por etapa do ritual (Pedagógicos - Decididos pelo
    // educador)
    /**
     * @brief Tempo para a etapa de molhar as mãos.
     * Uma preparação rápida para a aplicação do sabão.
     */
    constexpr unsigned long WET_TIMEOUT = 12000;

    /**
     * @brief Tempo para a aplicação de sabão.
     * Período destinado a garantir que a criança pegou sabão suficiente.
     */
    constexpr unsigned long SOAP_TIMEOUT = 12000;

    /**
     * @brief Tempo para a etapa de esfregar as mãos.
     * Baseado na recomendação da OMS/CDC de pelo menos 20 segundos para
     * eliminar patógenos.
     */
    constexpr unsigned long SCRUB_TIMEOUT = 25000;

    /**
     * @brief Tempo para o enxágue final.
     * Garante que todo o sabão e sujeira foram removidos.
     */
    constexpr unsigned long RINSE_TIMEOUT = 20000;

    /**
     * @brief Tempo para a secagem das mãos.
     * Etapa final importante para evitar a proliferação de bactérias em mãos
     * úmidas.
     */
    constexpr unsigned long DRY_TIMEOUT = 25000;

    /**
     * @brief Duração da comemoração final.
     * Tempo estendido para a criança celebrar a vitória com o robô.
     */
    constexpr unsigned long SUCCESS_DISPLAY_MS = 15000;

    /**
     * @brief Tempo necessário segurando o botão para entrar/sair do modo Debug.
     */
    constexpr unsigned long DEBUG_LONG_PRESS_MS = 1500;

    /**
     * @brief Tempo mínimo de pressão para considerar um clique válido
     * (Debounce). Evita que ruídos elétricos disparem múltiplos cliques.
     */
    constexpr unsigned long DEBUG_DEBOUNCE_MS = 50;

    /**
     * @brief Tempo segurando o botão antes de exibir o feedback visual de
     * "HOLD...".
     */
    constexpr unsigned long DEBUG_HOLD_FEEDBACK_MS = 500;

    /**
     * @brief Tempo que a mensagem "DEBUG OFF" permanece na tela antes de sumir.
     */
    constexpr unsigned long DEBUG_MSG_DURATION_MS = 2000;

    // --- COMPORTAMENTO (IDLE & BEHAVIOR) ---
    constexpr unsigned long IDLE_TIMEOUT_MS =
        120000; ///< Tempo máximo no estado ocioso antes de dormir (2 minutos)
    constexpr unsigned long IDLE_MIN_PAUSE_MS =
        8000; ///< Pausa mínima entre ações
    constexpr unsigned long IDLE_MAX_PAUSE_MS =
        15000; ///< Pausa máxima entre ações
    constexpr unsigned long IDLE_REMINDER_INTERVAL_MS =
        30000; ///< Intervalo entre ícones
    constexpr unsigned long IDLE_INITIAL_DELAY_MS =
        5000; ///< Atraso inicial ao entrar no Idle
    constexpr unsigned long INSTRUCTION_DISPLAY_MS =
        5000; ///< Tempo do ícone na tela

} // namespace GameConfig

/**
 * @namespace HardwareConfig
 * @brief Configurações gerais de hardware e memória.
 */
namespace HardwareConfig
{
    /**
     * @brief Tamanho máximo (em bytes) do buffer para leitura e filas de UID do
     * RFID.
     */
    constexpr uint8_t RFID_BUFFER_SIZE = 32;
} // namespace HardwareConfig
