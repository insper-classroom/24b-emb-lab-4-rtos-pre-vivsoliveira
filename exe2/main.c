#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

SemaphoreHandle_t xSemaphore_r;
SemaphoreHandle_t xSemaphore_g;

void led_1_task(void *p) {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    while (true) {
        // Espera o semáforo ser liberado pela task do botão
        if (xSemaphoreTake(xSemaphore_r, portMAX_DELAY) == pdTRUE) {
            gpio_put(LED_PIN_R, 1); // Acende o LED R
            vTaskDelay(pdMS_TO_TICKS(250));
            gpio_put(LED_PIN_R, 0); // Apaga o LED R
        }
    }
}

void btn_1_task(void *p) {
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    while (true) {
        // Verifica se o botão R foi pressionado
        if (!gpio_get(BTN_PIN_R)) {
            // Espera o botão ser solto
            while (!gpio_get(BTN_PIN_R)) {
                vTaskDelay(pdMS_TO_TICKS(10)); // Debounce
            }
            xSemaphoreGive(xSemaphore_r); // Libera o semáforo para acionar o LED R
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno delay para evitar polling contínuo
    }
}

void led_2_task(void *p) {
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    while (true) {
        // Espera o semáforo ser liberado pela task do botão G
        if (xSemaphoreTake(xSemaphore_g, portMAX_DELAY) == pdTRUE) {
            gpio_put(LED_PIN_G, 1); // Acende o LED G
            vTaskDelay(pdMS_TO_TICKS(250));
            gpio_put(LED_PIN_G, 0); // Apaga o LED G
        }
    }
}

void btn_2_task(void *p) {
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    while (true) {
        // Verifica se o botão G foi pressionado
        if (!gpio_get(BTN_PIN_G)) {
            // Espera o botão ser solto
            while (!gpio_get(BTN_PIN_G)) {
                vTaskDelay(pdMS_TO_TICKS(10)); // Debounce
            }
            xSemaphoreGive(xSemaphore_g); // Libera o semáforo para acionar o LED G
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno delay para evitar polling contínuo
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    // Inicializa os semáforos para os LEDs R e G
    xSemaphore_r = xSemaphoreCreateBinary();
    xSemaphore_g = xSemaphoreCreateBinary();

    // Cria as tasks para os LEDs e botões
    xTaskCreate(led_1_task, "LED_Task_R", 256, NULL, 1, NULL);
    xTaskCreate(btn_1_task, "BTN_Task_R", 256, NULL, 1, NULL);

    xTaskCreate(led_2_task, "LED_Task_G", 256, NULL, 1, NULL);
    xTaskCreate(btn_2_task, "BTN_Task_G", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
