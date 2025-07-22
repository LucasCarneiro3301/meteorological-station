#include "tasks.h"
#include "globals.h"
#include "wifi.h"
#include "pico/time.h"

void vTaskDisplay(void *params) {
    bool color = true;

    char str_tmp1[5];  // Buffer para armazenar a string
    char str_pa[5];  // Buffer para armazenar a string  
    char str_alt[5];  // Buffer para armazenar a string  
    char str_tmp2[5];  // Buffer para armazenar a string
    char str_umi[5];  // Buffer para armazenar a string
    char str_tmp3[5];  // Buffer para armazenar a string      


    while (true) {
        if (xSemaphoreTake(xDisplayMut, portMAX_DELAY) == pdTRUE) {
            sprintf(str_tmp1, "%.1fC", bmp280_temperature);
            sprintf(str_pa, "%.0fkPa", pressure);
            sprintf(str_alt, "%.0fm", altitude);
            sprintf(str_tmp2, "%.1fC", aht10_temperature);
            sprintf(str_umi, "%.1f%%", humidity);
            sprintf(str_tmp3, "%.1fC (avg)", avg_temperature);
        
            //  Atualiza o conteúdo do display com animações
            ssd1306_fill(&ssd, !color);                           // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, color, !color);       // Desenha um retângulo
            ssd1306_line(&ssd, 3, 15, 123, 15, color);            // Desenha uma linha
            ssd1306_line(&ssd, 3, 29, 123, 29, color);            // Desenha uma linha
            ssd1306_draw_string(&ssd, (status)?"NORMAL":"ERRO", 8, 6);         // Desenha uma string
            ssd1306_draw_string(&ssd, (connection)?"}":"~", 110, 6);         // Desenha uma string
            if(screen) {
                ssd1306_draw_string(&ssd, "BMP280  AHT10", 10, 19); // Desenha uma string
                ssd1306_line(&ssd, 63, 15, 63, 60, color);            // Desenha uma linha vertical
                ssd1306_draw_string(&ssd, str_tmp1, 14, 32);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_pa, 10, 43);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_alt, 14, 53);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_tmp2, 73, 32);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_umi, 73, 43);            // Desenha uma string
            } else {
                ssd1306_draw_string(&ssd, "GERAL", 50, 19); // Desenha uma string
                ssd1306_draw_string(&ssd, str_tmp3, 30, 32);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_pa, 10, 43);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_umi, 73, 43);             // Desenha uma string
                ssd1306_draw_string(&ssd, str_alt, 50, 53);            // Desenha uma string
            }
            ssd1306_send_data(&ssd);                            // Atualiza o display
        }

            ssd1306_send_data(&ssd);
            xSemaphoreGive(xDisplayMut);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
}
