#pragma once

/**
 * @enum RobotState
 * @brief Estados possíveis da Máquina de Estados (FSM) do Wash-Buddy.
 *
 * Este enum centraliza todos os comportamentos lógicos do robô, distinguindo
 * entre estados técnicos (BOOT, IDLE, ERROR), estados do ritual de lavagem
 * (WET, SOAP, SCRUB, RINSE, DRY) e estados de interação (WAITING, SUCCESS).
 */
enum class RobotState
{
    BOOT,    ///< Estado inicial de calibração de hardware e sensores.
    IDLE,    ///< Estado de repouso/espera por uma tag RFID (faucet/sabão).
    WET,     ///< Ritual: Molhar as mãos (feedback de chuva leve).
    SOAP,    ///< Ritual: Aplicar sabão (feedback de bolhas).
    SCRUB,   ///< Ritual: Esfregar as mãos (feedback de bolhas + movimento de
             ///< braços).
    RINSE,   ///< Ritual: Enxaguar as mãos (feedback de chuva forte).
    DRY,     ///< Ritual: Secar as mãos (feedback de vento).
    SUCCESS, ///< Celebração de conclusão do ritual (feedback de confete +
             ///< aceno).
    ERROR,   ///< Indicação de ação incorreta ou tag fora de ordem.
    WAITING, ///< Estado de transição aguardando a próxima ação do usuário.
    SLEEP    ///< Modo de baixo consumo (Deep Sleep) para economia de bateria.
};
