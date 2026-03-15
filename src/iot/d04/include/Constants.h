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
 * O leitor RFID é usado para identificar os objetos (sabão, toalha, etc.) via
 * tags.
 */
constexpr uint8_t SPI_SCK = 33;  ///< Pino de Clock (SCK)
constexpr uint8_t SPI_MISO = 26; ///< Pino Master In Slave Out (MISO)
constexpr uint8_t SPI_MOSI = 25; ///< Pino Master Out Slave In (MOSI)
constexpr uint8_t RFID_SDA = 32; ///< Pino de Seleção de Escravo (SS/SDA)
constexpr uint8_t RFID_RST = 27; ///< Pino de Reset do MFRC522

/**
 * @brief Pinos para comunicação I2C com o Display OLED (SSD1306).
 * O display é a principal interface visual, mostrando as expressões do robô.
 */
constexpr uint8_t OLED_SDA = 17; ///< Pino de Dados I2C
constexpr uint8_t OLED_SCL = 16; ///< Pino de Clock I2C

/**
 * @brief Pinos PWM para controle dos Servomotores.
 * Estes servos dão vida ao robô, movendo braços e cabeça para expressar
 * emoções.
 */
constexpr uint8_t SERVO_ARM_L =
    13; ///< Braço Esquerdo: Usado para acenar ou gesticular
constexpr uint8_t SERVO_ARM_R =
    12; ///< Braço Direito: Usado para acenar ou gesticular
constexpr uint8_t SERVO_HEAD =
    4; ///< Movimento da Cabeça: Permite olhar para os lados
} // namespace Pins

/**
 * @struct RFIDTags
 * @brief Identificadores únicos (UIDs) associados aos objetos físicos do
 * ritual.
 *
 * Cada tag RFID está colada em um objeto real usado pela criança durante a
 * lavagem das mãos.
 */
struct RFIDTags
{
        static constexpr const char* FAUCET =
            "6C:D3:D6:D4"; ///< Representa a Torneira (início/fim do enxágue)
        static constexpr const char* SOAP =
            "AC:F6:D8:D4"; ///< Representa o Sabonete
        static constexpr const char* SCRUB =
            "1C:D4:D8:D4"; ///< Representa a ação de esfregar as mãos
        static constexpr const char* TOWEL =
            "9C:ED:D9:D4"; ///< Representa a Toalha (finalização)
};

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
    15000; ///< Tempo máximo que o robô espera pela próxima ação (15s)
constexpr unsigned long ERROR_DISPLAY_MS =
    4000; ///< Duração da tela de erro/atenção (4s)

// Timeouts específicos por etapa do ritual
/**
 * @brief Tempo para a etapa de molhar as mãos.
 * Uma preparação rápida para a aplicação do sabão.
 */
constexpr unsigned long WET_TIMEOUT = 10000;

/**
 * @brief Tempo para a aplicação de sabão.
 * Período destinado a garantir que a criança pegou sabão suficiente.
 */
constexpr unsigned long SOAP_TIMEOUT = 10000;

/**
 * @brief Tempo para a etapa de esfregar as mãos.
 * Baseado na recomendação da OMS/CDC de pelo menos 20 segundos para eliminar
 * patógenos.
 */
constexpr unsigned long SCRUB_TIMEOUT = 25000;

/**
 * @brief Tempo para o enxágue final.
 * Garante que todo o sabão e sujeira foram removidos.
 */
constexpr unsigned long RINSE_TIMEOUT = 15000;

/**
 * @brief Tempo para a secagem das mãos.
 * Etapa final importante para evitar a proliferação de bactérias em mãos
 * úmidas.
 */
constexpr unsigned long DRY_TIMEOUT = 20000;

/**
 * @brief Duração da comemoração final.
 * Tempo em que o robô exibe alegria pelo sucesso do ritual concluído.
 */
constexpr unsigned long SUCCESS_DISPLAY = 6000;
} // namespace GameConfig
