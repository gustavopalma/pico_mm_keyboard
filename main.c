#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "tusb.h"
#include "bsp/board.h"

#include "usb_descriptors.h"

enum 
{
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500
};

int intervalo_ms = BLINK_NOT_MOUNTED;

enum botoes 
{
    GPIO_0 = 0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    TOTAL_BTN
};

void led_task();
void hid_usb_task();
void botoes_init();
void envia_dados_p_host(int report_id);

int main()
{
    board_init();
    tusb_init();
    botoes_init();
    cyw43_arch_init();
    while (true) {
        tud_task();
        led_task();
        hid_usb_task();
    }

    return 0;
}

void botoes_init() 
{
    for ( int i = 0; i < TOTAL_BTN; i++) 
    {
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
        gpio_set_pulls(i, true, false);
    }
}

void led_task()
{
    static uint32_t tempo_inicio = 0;
    static bool led_status = false;

    if ( board_millis() - tempo_inicio < intervalo_ms ) 
    {
        return; // não teve tempo suficiente pro led alterar o estado
    }

    tempo_inicio += intervalo_ms;

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_status);
    led_status = true - led_status;
}

// callbacks da usb
void tud_mount_cb() 
{
    intervalo_ms = BLINK_MOUNTED;
}

void tud_umount_cb()
{
    intervalo_ms  = BLINK_NOT_MOUNTED;
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    intervalo_ms = BLINK_SUSPENDED;
}

void tud_resume_cb()
{
    intervalo_ms = BLINK_MOUNTED;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  (void) instance;
  
  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    if (report_id == REPORT_ID_KEYBOARD)
    {
        if (bufsize < 1) return;

        uint8_t const teclado_leds = buffer[0];

        if ( teclado_leds & KEYBOARD_LED_CAPSLOCK ) 
        {
            intervalo_ms = 0;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        } else {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
            intervalo_ms = BLINK_MOUNTED;
        }
        
    }
  }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
   (void) instance;
   (void) report_id;
   (void) report_type;
   (void) buffer;
   (void) reqlen;

   return 0; 
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
   (void) instance;
   (void) len;

   uint8_t proximo_id = report[0] + 1;

   if ( proximo_id < REPORT_ID_COUNT )
   {
    envia_dados_p_host(proximo_id);
   }
}

void envia_dados_p_host(int report_id)
{
    if (report_id == REPORT_ID_KEYBOARD) 
    {
        static bool tem_tecla = false;
        int btn = board_button_read();
        if ( btn && !tem_tecla) 
        {
            uint8_t teclas[6] = {0};
            teclas[0] = HID_KEY_M;
            teclas[1] = HID_KEY_A;
            teclas[2] = HID_KEY_K;
            teclas[3] = HID_KEY_E;
            teclas[4] = HID_KEY_ENTER;
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, teclas);
            tem_tecla = true;
        }
        else if (!btn && tem_tecla)
        {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
            tem_tecla = false;
        }
    } else if ( report_id == REPORT_ID_CONSUMER_CONTROL ) {
        static bool tem_mult = false;
        uint16_t cmd = 0;

        while ( gpio_get(GPIO_0) == 0 ) 
        {
            cmd = HID_USAGE_CONSUMER_AC_HOME;
        }

        if ( gpio_get(GPIO_1) == 0 )
        {
            cmd = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
        }

        if ( gpio_get(GPIO_2) == 0 ) 
        {
            cmd = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        }

        while ( gpio_get(GPIO_3) == 0) 
        {
            cmd = HID_USAGE_CONSUMER_PLAY_PAUSE;
        }

        if ( cmd && !tem_mult )
        {
            tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &cmd, 2);
            tem_mult = true;
            cmd = 0;
        } else if ( tem_mult ) {
            uint16_t tecla_vazia = 0;
            tem_mult = false;
            cmd = 0;
            tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &tecla_vazia, 2);
        }
    }
}

void hid_usb_task() 
{
    const int intervalo_hid_ms = 10;
    static int tempo_inicio = 0;


    //desativa a função de BLINK
    if (intervalo_ms == 0) 
    {
        return;
    }

    if (board_millis() - tempo_inicio < intervalo_hid_ms)
    {
        return; //tempo menor que o estipulado;
    }

    //enviar os dados do teclado
    int btn = board_button_read();
    envia_dados_p_host(REPORT_ID_KEYBOARD);
    envia_dados_p_host(REPORT_ID_CONSUMER_CONTROL);
}