#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

const int BTN_PIN = 28;
const int LED_PIN = 4;

volatile int led_state = 0;
volatile int pisca = 0;
repeating_timer_t timer;

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) {
        pisca = !pisca;
    }
}

bool timer_callback(repeating_timer_t *rt) {
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);
    return true;
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    
    while (true) {
        if (pisca && !timer.alarm_id) {
            add_repeating_timer_ms(500, timer_callback, NULL, &timer);
        }

        if (!pisca) {
            cancel_repeating_timer(&timer);
            gpio_put(LED_PIN, 0);
            led_state = 0;
        }

        sleep_ms(50);
    }
}
