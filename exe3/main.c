#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

const int BTN_PIN = 28;
const int LED_PIN = 4;

volatile int led_state = 0;
volatile absolute_time_t tempo_abs;
volatile int btn = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) {
        tempo_abs = get_absolute_time();
        btn = 1;
    } else if (events == GPIO_IRQ_EDGE_RISE) {
        int duracao = absolute_time_diff_us(tempo_abs, get_absolute_time());
        if (duracao >= 500000) {
            led_state = !led_state;
            gpio_put(LED_PIN, led_state);
        }
        btn = 0;
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    
    while (true) {
        sleep_ms(50);
    }
}
