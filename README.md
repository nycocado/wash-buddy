# Wash Buddy

O Wash Buddy é um brinquedo de banho inteligente projetado para incentivar e melhorar os hábitos de higiene das mãos para crianças por meio de interação física e feedback háptico. Ao detetar pressão e movimento, o dispositivo fornece feedback em tempo real para garantir uma experiência de lavagem das mãos ou do banho adequada e envolvente.

## Visão Geral

O sistema utiliza um microcontrolador ESP32-C3 integrado com sensores de tensão (strain gauges) e um sensor inercial (MPU-6050) para reconhecer quando o utilizador aperta e roda o brinquedo. Ao detetar a interação combinada correta, um motor de vibração é ativado. O dispositivo funciona num ciclo ativo de 20 segundos para promover o tempo de lavagem recomendado e possui conectividade Bluetooth Low Energy (BLE) para transmitir dados de utilização para uma aplicação móvel voltada para os pais.

## Funcionalidades

* **Deteção de Interação:** Identifica o aperto físico (pressão) e os movimentos de rotação ou fricção.
* **Feedback Háptico:** Ativa um motor de vibração apenas quando a pressão e a rotação são detetadas simultaneamente.
* **Cronómetro Inteligente:** Gere um ciclo ativo de 20 segundos, pausando durante a inatividade e reiniciando o ciclo após 5 segundos sem interação.
* **Transmissão de Dados BLE:** Atua como um Servidor GATT para disponibilizar dados de higiene para acesso remoto através de uma aplicação móvel.
* **Design Estanque (IP68):** Invólucro totalmente selado, garantindo uma operação segura durante a imersão em água e exposição ao sabão.
* **Eficiência Energética:** Entra em modo de suspensão profunda (deep sleep) entre utilizações, acordando através de interrupções de hardware para conservar a vida útil da bateria.

## Arquitetura de Hardware

O núcleo do hardware baseia-se nos seguintes componentes:
* **Microcontrolador:** ESP32-C3 HW-466AB (com suporte BLE)
* **Sensores de Pressão:** 8x Extensómetros (Strain Gauges) com Amplificador HX711
* **Sensor de Movimento:** MPU-6050 (Giroscópio/Acelerómetro)
* **Atuador:** Motor de Vibração (acionado por um Transístor 2N2222A)
* **Sistema de Alimentação:** Bateria LiPo 3.7V 1800mAh com módulo de carregamento selado (indutivo ou magnético)

O ESP32 processa os sinais dos periféricos de entrada (via GPIO e I2C), avalia as regras de interação e aciona a porta PWM para o motor de vibração, enquanto gere as comunicações BLE.

## Estrutura do Repositório

```
.
├── media/               # Esquemas, diagramas de arquitetura e imagens do protótipo
├── src/
│   ├── gui/app/         # Aplicação móvel Android (Kotlin) para pais
│   └── iot/             # Firmware do dispositivo (Arduino/C++)
├── report.pdf           # Relatório detalhado do projeto
└── slides.pdf           # Slides da apresentação do projeto
```

## Trabalhos Futuros

* **Aplicação Móvel:** Desenvolvimento completo da app GATT Client para visualização do histórico de banhos.
* **Interface de Carregamento:** Definição final entre carregamento indutivo ou pogo pins magnéticos para a interface estanque.
* **Gamificação:** Implementação de um sistema de recompensas digital baseado nos dados de uso.
* **Atualizações OTA:** Suporte para atualizações de firmware Over-The-Air para calibração remota de sensores.

## Equipa

* Nycolas Souza
* Luan Ribeiro
* Lohanne Guedes
* Kira Sousa

---
*Projeto desenvolvido para a disciplina de Project Factory - 2026*
