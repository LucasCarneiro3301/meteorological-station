#include "tasks.h"
#include "globals.h"
#include "ws2812.h"
#include "buzzer.h"

void jingle() {
    for (int i = 0; i < 2; i++) {
        play_buzzer(RE);
        vTaskDelay(pdMS_TO_TICKS(300));
        
        stop_buzzer();
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    stop_buzzer();
    vTaskDelay(pdMS_TO_TICKS(600));
}

void vTaskAlert(void *params) {
    bool last_stt = true;
    PIO pio = pio0;
    int sm = 0;

    uint offset = pio_add_program(pio, &ws2812_program);
  
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW); //Inicializa a matriz de leds

    gpio_init(RED);
    gpio_set_dir(RED, GPIO_OUT);
    
    gpio_init(GREEN);
    gpio_set_dir(GREEN, GPIO_OUT);

    symbol('*');        // Apaga a matriz

    while (true)
    {
        if(status && status != last_stt) {
            gpio_put(GREEN, true);
            gpio_put(RED, false);
            symbol('v');
        } else if (!status && status != last_stt) {
            gpio_put(GREEN, false);
            gpio_put(RED, true);
            jingle();
            symbol('x');
        }

        last_stt = status;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}