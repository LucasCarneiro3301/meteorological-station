#include "wifi.h"
#include "tasks.h"
#include "server.h"
#include "globals.h"

void ssd1306_msg() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Falha na", 18, 28);
    ssd1306_draw_string(&ssd, "conexao...", 18, 48);
    ssd1306_send_data(&ssd);
    sleep_ms(800);
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Aguarde", 18, 28);
    ssd1306_draw_string(&ssd, "um momento...", 18, 48);
    ssd1306_send_data(&ssd);
    sleep_ms(800);
}


void vTaskWiFi(void *params)
{
    // Inicialização do Wi-Fi
    printf("Inicializando Wi-Fi...\n");
    while(cyw43_arch_init()) {
        printf("ERRO: Falha ao inicializar o Wi-Fi! Tentando novamente em 5s...\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    cyw43_arch_gpio_put(LED_PIN, 0);    // LED apagado inicialmente
    cyw43_arch_enable_sta_mode();       // Modo Station

    printf("Conectando à rede: %s\n", WIFI_SSID);
    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 1000)) {
        printf("ERRO: Falha ao conectar ao Wi-Fi. Tentando novamente...\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    connection = true;
    printf("Conectado com sucesso!\n");

    // Caso seja a interface de rede padrão - imprimir o IP do dispositivo.
    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }
    
    start_http_server();

    while (true)
    {
        int link = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

        if (
            (link < 0) || (link == CYW43_LINK_DOWN) || (link == CYW43_LINK_NONET) ||
            !cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) || 
            (netif_ip4_addr(netif_default)->addr == 0)
        ) {
            connection = false;
            printf("Desconectado! Tentando reconectar...\n");

            cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA); // Libera conexão atual
            int ret = cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK);

            if (ret != PICO_OK) {
                printf("ERRO: Reconexão falhou! Tentando novamente em 5s...\n");
                if (xSemaphoreTake(xDisplayMut, portMAX_DELAY) == pdTRUE) {
                    ssd1306_msg();
                    xSemaphoreGive(xDisplayMut);
                }
                vTaskDelay(pdMS_TO_TICKS(5000));
            } else {
                connection = true;
                printf("Reconectado com sucesso!\n");
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        } else {
            cyw43_arch_poll();
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

    cyw43_arch_deinit();
}
