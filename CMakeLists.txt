


cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

set(FREERTOS_KERNEL_PATH "C:/FreeRTOS-Kernel") # Adiciona o FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(meteorological-station C CXX ASM) 
pico_sdk_init()


include_directories(${CMAKE_SOURCE_DIR}/libs/include) # Adiciona o diretório 'libs/include' à lista global de diretórios de headers 
include_directories(${CMAKE_SOURCE_DIR}/libs/include/ws2812) # Adiciona o diretório 'libs/include' à lista global de diretórios de headers 
include_directories(${CMAKE_SOURCE_DIR}/libs/include/ssd1306) # Adiciona o diretório 'libs/include' à lista global de diretórios de headers 

add_executable(${PROJECT_NAME}  
        main.c
        libs/source/server.c 
        libs/source/buzzer.c 
        libs/source/ssd1306.c
        libs/source/ws2812.c
        libs/source/tasks/wifi_task.c
        libs/source/tasks/sensors_task.c
        libs/source/tasks/display_task.c
        libs/source/tasks/alert_task.c
        libs/source/sensors/aht20.c
        libs/source/sensors/bmp280.c
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})  # Adiciona o diretório raiz como include privado apenas para o target atual

pico_generate_pio_header(${PROJECT_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/include/ws2812/ws2812.pio 
    OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated
)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_gpio
    hardware_adc
    hardware_i2c
    hardware_pwm
    hardware_clocks
    hardware_irq
    hardware_pio
    hardware_timer
    pico_bootrom
    pico_cyw43_arch_lwip_threadsafe_background
    FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


pico_add_extra_outputs(${PROJECT_NAME})