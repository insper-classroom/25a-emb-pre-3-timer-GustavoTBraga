#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;
const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int led_state_r = 0;
volatile int led_state_g = 0;
volatile int pisca_r = 0;
volatile int pisca_g = 0;
repeating_timer_t timer_r;
repeating_timer_t timer_g;

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) {
        if (gpio == BTN_PIN_R) {
            pisca_r = !pisca_r;
        } else if (gpio == BTN_PIN_G) {
            pisca_g = !pisca_g;
        }
    }
}

bool timer_callback(repeating_timer_t *rt) {
    if (rt == &timer_r && pisca_r) {
        led_state_r = !led_state_r;
        gpio_put(LED_PIN_R, led_state_r);
    } else if (rt == &timer_g && pisca_g) {
        led_state_g = !led_state_g;
        gpio_put(LED_PIN_G, led_state_g);
    }
    return true;
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);
    
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);
    
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);
    
    while (true) {
        if (pisca_r && !timer_r.alarm_id) {
            add_repeating_timer_ms(500, timer_callback, NULL, &timer_r);
        }
        
        if (pisca_g && !timer_g.alarm_id) {
            add_repeating_timer_ms(250, timer_callback, NULL, &timer_g);
        }

        if (!pisca_r) {
            cancel_repeating_timer(&timer_r);
            gpio_put(LED_PIN_R, 0);
            led_state_r = 0;
        }

        if (!pisca_g) {
            cancel_repeating_timer(&timer_g);
            gpio_put(LED_PIN_G, 0);
            led_state_g = 0;
        }
        sleep_ms(50);
    }
}
