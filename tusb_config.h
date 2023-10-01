#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

// Configuração do modo DEVICE na Raspberry
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

//Configurando a Placa como Raspberry Pi Pico (w)
#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS OPT_OS_PICO
#endif

//Configura o tamanho do endpoint da USB
#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE 64
#endif

//Habilita o Modo HID no dispositivo USB
#define CFG_TUD_HID 1

//Define o tamanho do buffer do dispositivo USB
#define CFG_TUD_HID_BUFSIZE 32

#endif 