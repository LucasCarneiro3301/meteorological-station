#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "server.h"   
#include "buzzer.h"
#include "globals.h"
#include "tasks.h"
#include "wifi.h"
#include "pico/bootrom.h"


//Os Dados de wifi são definidos em wifi_task.h

SemaphoreHandle_t xDisplayMut;

ssd1306_t ssd;
float aht10_temperature, bmp280_temperature, avg_temperature, pressure, humidity, altitude;
float minAHT10 = 18.0f, maxAHT10 = 25.0, offsetAHT10 = 0, minBMP280 = 18.0f, maxBMP280 = 25.0f, offsetBMP280 = 0;
float minUmid = 50.0f, maxUmid = 60.0f, offsetUmid = 0.0f;
float minPress = 98*1000, maxPress = 100*1000, offsetPress = 0.0f;
bool connection = false, status = true, screen = true;
char msg[256];

volatile uint32_t last_time = 0;        // Armazena o tempo do último evento (em microssegundos)

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());  // Obtém o tempo atual em microssegundos
  
    if (current_time - last_time > 2e5) { // 200 ms de debouncing
        last_time = current_time; 
        if(gpio == BTNA) { // Botão A alterna entre diferentes telas ou retorna para as telas de seleção
            screen = !screen;
        } else if(gpio == BTNB) {
            offsetAHT10  = 0;
            offsetBMP280 = 0;
            offsetUmid   = 0;
            offsetPress  = 0;
        } else if(gpio == BTNJ) {
            reset_usb_boot(0, 0);
        }
    }
}

int main()
{
    // --- Inicialização do I2C ---
    i2c_init(I2C_PORT_1, 400 * 1000);
    gpio_set_function(I2C_SDA_1, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_1, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_1);
    gpio_pull_up(I2C_SCL_1);

    stdio_init_all();

    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);
    gpio_set_irq_enabled_with_callback(BTNB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BTNJ);
    gpio_set_dir(BTNJ, GPIO_IN);
    gpio_pull_up(BTNJ);
    gpio_set_irq_enabled_with_callback(BTNJ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    strcat(msg, "0");

    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, DISPLAY_ADDRESS, I2C_PORT_1);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Iniciando...", 18, 28);
    ssd1306_send_data(&ssd);

    sleep_ms(1000); // Delay inicial

    xDisplayMut = xSemaphoreCreateMutex();

    printf("Heap livre: %u bytes\n", xPortGetFreeHeapSize());

    
    xTaskCreate(vTaskWiFi, "Wi-Fi Task", configMINIMAL_STACK_SIZE + 512, NULL, 1, NULL);                // Controla a conexão Wi-Fi
    xTaskCreate(vTaskDisplay, "Display Task", configMINIMAL_STACK_SIZE + 512, NULL, 1, NULL);           // Controla o display ssd1306
    xTaskCreate(vTaskSensors, "Level Resistor Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);    // Mede temperatura, pressão e umidade
    xTaskCreate(vTaskAlert, "Matrix Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);             // Utiliza a matriz de leds
    // xTaskCreate(vTaskButton, "Button Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);             // Configura Limites de nível
    vTaskStartScheduler();
    panic_unsupported();

    return 0;
}