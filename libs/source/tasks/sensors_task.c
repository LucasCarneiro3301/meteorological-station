#include "tasks.h"
#include "globals.h"
#include "aht20.h"
#include "bmp280.h"
#include <math.h>
#include <string.h>


void vTaskSensors(void *params)
{
    struct bmp280_calib_param bmp280_params;
    AHT20_Data data;
    int32_t raw_temp_bmp, raw_pressure;

    // Estabelece a comunicação I2C com os sensores
    i2c_init(I2C_PORT_0, 400 * 1000);
    gpio_set_function(I2C_SDA_0, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_0, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_0);
    gpio_pull_up(I2C_SCL_0);

    // Inicializa o BMP280
    bmp280_init(I2C_PORT_0);
    bmp280_get_calib_params(I2C_PORT_0, &bmp280_params);

    // Inicializa o AHT10/20
    aht20_reset(I2C_PORT_0);
    aht20_init(I2C_PORT_0);
    
    while (true)
    { 
        bmp280_read_raw(I2C_PORT_0, &raw_temp_bmp, &raw_pressure);  // Leitura do BMP280
      
        // Leitura do AHT10/20 (Temperatura e umidade)
        if (!aht20_read(I2C_PORT_0, &data))
        {
            printf("Erro na leitura do AHT10!\n\n\n");
        }

        pressure = (bmp280_convert_pressure(raw_pressure, raw_temp_bmp, &bmp280_params))/1000.0 + offsetPress/1000; // Pressão
        aht10_temperature = data.temperature + offsetAHT10;                                                         // Temperatura (AHT10)
        bmp280_temperature = bmp280_convert_temp(raw_temp_bmp, &bmp280_params)/100.0 + offsetBMP280;                // Temperatura (BMP280)
        humidity = data.humidity + offsetUmid;                                                                      // Umidade
        avg_temperature = (bmp280_temperature + aht10_temperature)/2;                                               // Média aritmética entre os valores de temperatura
        altitude = (44330.0 * (1.0 - pow((pressure*1000.0) / SEA_LEVEL_PRESSURE, 0.1903)));                         // Altitude

        // Limites
        msg[0] = '\0';  // Zera a string

        if (aht10_temperature < minAHT10)
            strcat(msg, "Temperatura do AHT20 abaixo do limite minimo. ");
        else if (aht10_temperature > maxAHT10)
            strcat(msg, "Temperatura do AHT20 acima do limite maximo. ");

        if (bmp280_temperature < minBMP280)
            strcat(msg, "Temperatura do BMP280 abaixo do limite minimo. ");
        else if (bmp280_temperature > maxBMP280)
            strcat(msg, "Temperatura do BMP280 acima do limite maximo. ");

        if (humidity < minUmid)
            strcat(msg, "Umidade abaixo do limite minimo.");
        else if (humidity > maxUmid)
            strcat(msg, "Umidade acima do limite maximo. ");

        if (pressure < minPress / 1000.0)
            strcat(msg, "Pressão abaixo do limite minimo. ");
        else if (pressure > maxPress / 1000.0)
            strcat(msg, "Pressão acima do limite maximo. ");

        if (!(strlen(msg) > 0)) {
            status = true;
            strcat(msg, "Situação normal. Nenhum desvio encontrado.");
        } else {
            status = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}