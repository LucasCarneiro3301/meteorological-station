# meteorological-station
Este projeto integra sensores I2C à plataforma BitDogLab com o objetivo de criar uma estação meteorológica embarcada, conectada via Wi-Fi, com visualização em display OLED e interface web interativa.

__Aluno:__
Lucas Carneiro de Araújo Lima

## ATIVIDADE 

__Para este trabalho, os seguintes componentes e ferramentas se fazem necessários:__
1) Microcontrolador Raspberry Pi Pico W.
2) Ambiente de trabalho VSCode.
3) LEDs RGB.
4) _Display_ SSD1306.
5) 3 Botões Pull-Up.
7) Ferramenta educacional BitDogLab.
8) FreeRTOS
9) BMP280
10) ATH10
11) Extensor i2c

__O resultado do projeto pode ser assistido através deste link: [Vídeo de Apresentação](https://youtu.be/WnFsIlx2fj0).__

## Instruções de Uso

### 1. Clone o repositório
Abra o terminal e execute o comando abaixo para clonar o repositório em sua máquina:
```bash
https://github.com/LucasCarneiro3301/meteorological-station.git
```

### 2. Configure o ambiente de desenvolvimento
Certifique-se de que o [SDK do Raspberry Pi Pico](https://github.com/raspberrypi/pico-sdk) esteja instalado e configurado corretamente no seu sistema.

### 3. Instale o CMake
Certifique-se de que o [CMake](https://cmake.org/download/) esteja instalado e configurado corretamente no seu sistema.

### 4. Clone o repositório do FreeRTOS
Certifique-se de clonar o repositório do FreeRTOS. Repositório: [FreeRTOS](https://github.com/FreeRTOS/FreeRTOS-Kernel.git).

### 5. Conexão com a Rapberry Pico
1. Conecte o Raspberry Pi Pico ao seu computador via USB.
2. Inicie o modo de gravação pressionando o botão **BOOTSEL** e **RESTART**.
3. O Pico será montado como um dispositivo de armazenamento USB.
4. Execute através do comando **RUN** a fim de copiar o arquivo `meteorological-station.uf2` para o Pico.
5. O Pico reiniciará automaticamente e executará o programa.

### 5. Observações (IMPORTANTE !!!)
2. Manuseie a placa com cuidado.
