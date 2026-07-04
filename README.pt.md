<div align="center">

<img src="media/photos/final-version-01.png">

# Wash Buddy

Um companheiro robótico interativo que gamifica a lavagem das mãos das crianças, guiando cada etapa através de expressões faciais, movimento orgânico e feedback sonoro despoletado por RFID.

[![License: CC BY-NC 4.0](https://img.shields.io/badge/License-CC%20BY--NC%204.0-lightgrey.svg)](LICENSE)
![Status](https://img.shields.io/badge/status-concluído-6f42c1)

[![C++](https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-F5822A?logo=platformio&logoColor=white)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Arduino-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)

[English](README.md) | Portuguese

</div>

## Sobre

O **Wash Buddy** é um robô interativo totalmente standalone: a criança aproxima brinquedos com tags RFID (sabão, esponja, toalha) do robô para avançar no ciclo de lavagem, e ele responde nativamente através de um rosto em ecrã OLED, falas de voz sincronizadas e gestos de braços/cabeça — sem depender de smartphone ou app.

O design foi desenvolvido em estreita colaboração com a equipa de Design **D04**, aliando uma PCB customizada de dois andares a uma arquitetura de firmware pensada para feedback expressivo em tempo real.

## Como funciona

O ciclo de lavagem é uma sequência linear rigorosa, uma tag física por etapa: **Idle → Wet → Soap → Scrub → Rinse → Dry → Success**. Quatro tags RFID conduzem-no — `FAUCET` (início/enxágue), `SOAP`, `SCRUB` e `TOWEL` — cada uma encapsulada num objeto lúdico real.

1. **Idle.** O robô espera, piscando e olhando em volta. A cada 30s de inatividade, mostra um ícone de torneira como lembrete. Após 2 minutos sem nenhuma tag, desliga-se sozinho para poupar bateria.
2. **Validação de tags.** Cada estado do ritual espera exatamente uma tag de "repetição" (a que abriu o estado) e uma tag "seguinte" (a que avança o ciclo). Ler a tag _atual_ de novo repete a etapa; ler a tag _seguinte_ avança; qualquer outra coisa — tag errada, ordem errada — cai diretamente num estado de `ERROR` com áudio corretivo e sinal visual.
3. **Limite de repetição.** A criança só pode repetir uma etapa uma vez antes de o robô forçar `ERROR` — isto impede que o ritual fique preso indefinidamente numa única etapa, mas ainda tolera uma tentativa honesta de repetição.
4. **Timeouts por etapa.** Cada etapa tem o seu próprio limite de tempo ajustado à tarefa: Wet/Soap 12s, Scrub 25s (o mínimo de Scrub segue a recomendação da OMS/CDC de 20 segundos para lavagem das mãos), Rinse 20s, Dry 25s. Ao expirar, entra-se em `WAITING`, que estimula a criança com animações de olhar-em-volta, um humor "triste" após 15s, e um lembrete sonoro repetido a cada 15s, durante até 45s antes de desistir.
5. **Sucesso e reset.** Concluir o Dry dispara uma celebração de confetes e aceno, e depois reinicia o progresso do ritual para que a próxima criança possa começar do zero a partir do Idle.

Um botão físico de debug ignora tudo isto para fins de teste: uma pressão longa (1,5s) ativa uma sobreposição de debug, e pressões curtas seguintes percorrem ciclicamente todos os estados por ordem — usado nas sessões de design para afinar coreografia e áudio sem precisar de uma tag real.

## Funcionalidades

- **Rosto OLED expressivo** — olhos, piscar e olhar-em-volta no estado de espera são calculados processualmente a cerca de 50 FPS pelo `ExpressionEngine` (baseado na biblioteca `esp32-eyes`), sem recorrer a animações pré-renderizadas.
- **Personalidade orgânica por vinhetas** — o `BehaviorEngine` reproduz "vinhetas" (`BehaviorVignette`): um humor, uma direção de olhar, coreografia de servos por eixo e um som opcional, escolhidos de um conjunto próprio por estado com pausas de repouso aleatórias. Pausa-se automaticamente sempre que há um ícone de instrução no ecrã, para nunca distrair da etapa atual.
- **Motor de partículas customizado** — um conjunto fixo de 40 partículas renderiza bolhas, chuva fina/forte, confetes e vento, cada efeito com probabilidade de geração e limite próprios por `EffectType`.
- **Movimentação orgânica** — três servomotores SG90 (cabeça + dois braços), cada um modelado como um `MotorAxis` independente com _easing_ smoothstep, _clamping_ de ângulo por eixo e inversão opcional esquerda/direita para coreografias espelhadas dos braços.
- **Driver de áudio resiliente** — o `AudioController` corre a sua própria máquina de estados de inicialização (`WAKING_UP → CONNECTING → SETTING_MODE → READY`) sobre o DFPlayer Pro via UART, para que um arranque lento do player nunca bloqueie o loop principal; o volume acompanha um potenciómetro com leituras ADC filtradas por debounce.
- **Desligamento automático físico** — um MOSFET corta a alimentação dos servos e, no desligamento, de todo o sistema através do pino `KEY` do módulo carregador — eliminando o consumo fantasma dos motores em repouso e preservando a bateria entre sessões.

## Arquitetura

**Software** — C++ modular (framework Arduino via PlatformIO), separado numa camada de abstração de hardware (RFID, servos, áudio, energia) e numa camada de comportamento/interface (partículas, animação facial, movimento espontâneo):

- **State pattern** governa a máquina de estados do ciclo de lavagem (`include/states/`, uma classe por etapa, enum `RobotState`) através do `GameController`, que mantém um conjunto estático de instâncias de estado pré-alocadas (sem `new`/`delete` em tempo de execução durante o uso) e centraliza as regras de limite de repetição e timeout descritas acima.
- **Options pattern** na configuração dos controladores (ex.: `AudioSettings`, `MotionSettings`, `DisplayConfig`), evitando _magic numbers_ espalhados pelo código.
- **Divisão dual-core via FreeRTOS** — o Core 0 executa exclusivamente o `DisplayTask`, correndo o `DisplayOrchestrator` a um intervalo fixo de ~60 FPS para que a renderização do rosto e as partículas nunca engasguem; o Core 1 executa o `loop()` do Arduino (lógica de estados, cinemática dos servos) mais uma `RFIDTask` e `audioTask` dedicadas, desacopladas do `loop()` do Core 1 através de uma fila FreeRTOS para os UIDs das tags.
- **`DisplayOrchestrator`** compõe o `ExpressionEngine` (olhos) e o `ParticleSystem` num único buffer da U8g2, com uma transição em cortina (abrir/fechar) sempre que um ícone de instrução substitui o rosto, para que nada apareça ou desapareça de forma abrupta.

**Hardware** — um ESP32-WROOM-32 orquestra os periféricos (RC522 via SPI, OLED SH1106 via I2C a 800 kHz, DFPlayer Pro via UART, servos via PWM) numa PCB customizada de dois andares empilháveis: um módulo inferior de potência (carregador DFR1026 + isolamento por MOSFET) e um módulo superior de lógica, fisicamente separados para que o ruído elétrico dos motores não corrompa os barramentos SPI/I2C. Os servos são alimentados diretamente pela bateria (contornando o limite de corrente do módulo carregador), controlados por um MOSFET que elimina o consumo fantasma do _holding torque_ em repouso; o mesmo padrão de MOSFET aciona o auto-shutdown, puxando o pino `KEY` do módulo carregador para GND.

| Componente                     | Função                           |
| ------------------------------ | -------------------------------- |
| ESP32-WROOM-32                 | Microcontrolador principal       |
| RC522                          | Leitor RFID (tags 13.56 MHz)     |
| OLED 1.3" 128×64 (SH1106, I2C) | Ecrã de rosto / expressões       |
| DFPlayer Pro                   | Reprodução de áudio + 2× colunas |
| 3× Servo SG90                  | Atuadores de cabeça e braços     |
| DFR1026                        | Módulo de carregamento/energia   |
| 2× LiPo 2200 mAh               | Alimentação (4400 mAh no total)  |

<table>
<tr>
<td width="50%"><img src="media/photos/power-01.png" width="100%"><br>Camada de potência — ligações soldadas à mão do carregador e do circuito de isolamento por MOSFET</td>
<td width="50%"><img src="media/photos/logic-01.png" width="100%"><br>Camada de lógica — ESP32-WROOM-32 e DFPlayer Pro</td>
</tr>
<tr>
<td width="50%"><img src="media/photos/power-02.png" width="100%"><br>Camada de potência — capacitores de filtro de 1000 µF e dissipador do MOSFET</td>
<td width="50%"><img src="media/photos/logic-02.png" width="100%"><br>Camada de lógica — portas USB-C para programação do ESP32 e configuração do DFPlayer Pro</td>
</tr>
<tr>
<td width="50%"><img src="media/photos/power-03.png" width="100%"><br>Camada de potência, vista lateral</td>
<td width="50%"><img src="media/photos/logic-03.png" width="100%"><br>Camada de lógica, vista lateral</td>
</tr>
<tr>
<td colspan="2"><img src="media/photos/stack-01.png" width="100%"><br>As duas camadas montadas — placa lógica em cima, placa de potência em baixo</td>
</tr>
</table>

## Requisitos

| Ferramenta                            | Versão mínima |
| ------------------------------------- | ------------- |
| [PlatformIO](https://platformio.org/) | Core 6+       |

As dependências de placa e bibliotecas (`MFRC522`, `ESP32Servo`, `U8g2`, `DFRobot_DF1201S`) estão declaradas no `platformio.ini` e são instaladas automaticamente pelo PlatformIO.

## Como executar

```bash
cd src/iot
pio run --target upload   # compila e grava no ESP32
pio device monitor        # log série a 115200 baud
```

## Testes

```bash
cd src/iot
pio test -e esp32dev
```

A suíte Unity (`test/test_logic/`) corre no próprio dispositivo e cobre a matemática de _easing_ do movimento, o _clamping_/inversão dos eixos dos servos e a sequência de passos da coreografia.

## Estrutura do projeto

```
wash-buddy/
├── src/iot/            # Projeto de firmware PlatformIO
│   ├── include/        # Cabeçalhos globais, cabeçalhos por estado, constantes de pinos/áudio
│   ├── lib/             # Motores e controladores (Behavior, Particle, Expression,
│   │                     #   Motion, Audio, Power, RFID, Display)
│   ├── src/             # Ponto de entrada main.cpp, GameController, implementação dos estados
│   └── test/            # Suíte de testes Unity
├── media/               # Relatórios das milestones, BOMs, esquemas elétricos, fotos, sons
└── LICENSE
```

## Documentação

#### Milestone 1

- [Relatório](media/milestone-1/report.pdf) — proposta e requisitos iniciais do projeto.

#### Milestone 2

- [Relatório](media/milestone-2/report.pdf) — arquitetura, racional de hardware e divisão de tarefas.
- [Esquema elétrico (D04)](media/milestone-2/circuit-d04.pdf)
- [Bill of materials (D04)](media/milestone-2/bom-d04.xlsx)
- [Manual de montagem](media/milestone-2/assembly-manual.png)
- [Foto do modelo físico](media/milestone-2/physical-model.jpeg)
- [Vídeo de demonstração](media/milestone-2/video.mp4)

#### Sons

- [Biblioteca de sons](media/sounds/) — todas as falas e efeitos sonoros usados pelo firmware.

#### Fotos

- [Fotos](media/photos)

## Equipa

- [Nycolas Souza](https://github.com/nycocado) — firmware (C++/PlatformIO), paralelização via FreeRTOS, motor visual e matemática da coreografia dos servos.
- [Luan Ribeiro](https://github.com/Ninjaok) — engenharia de hardware e desenho do esquema elétrico; isolamento por MOSFET e circuito de auto-shutdown.
- [Lohanne Guedes](https://github.com/lohanneguedes) — prototipagem física e integração de laboratório; soldadura da PCB e montagem do chassi.
- [Kira Sousa](https://github.com/Kira-Sousa) — regras de negócio da máquina de estados e sound design; curadoria e integração do áudio.

## Licença

Distribuído sob a licença **CC BY-NC 4.0**, © 2026 Nycolas Souza, Luan Ribeiro, Lohanne Guedes, Kira Sousa.

Permite a partilha e adaptação do trabalho com atribuição, exclusivamente para fins não comerciais. O texto completo está em [LICENSE](LICENSE).
