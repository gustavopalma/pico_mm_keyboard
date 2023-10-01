#include "tusb.h"
#include "usb_descriptors.h"

#define USB_VID 0xCAFE
#define USB_PID 0x2023

//descritor do dispositivo, para identificação no HOST
tusb_desc_device_t const desc_device =
{
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01
};

//callback para envio do descritor do dispositivo
uint8_t const *tud_descriptor_device_cb(void) 
{
    return (uint8_t const *) &desc_device;
}

//tipos de report (envio) que serão feitos ao host
uint8_t const desc_hid_report[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL))
};

//callback para enviar o tipo de report feitos ao host
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    return desc_hid_report;
}

enum 
{
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration[] = 
{
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), 0x81, CFG_TUD_HID_BUFSIZE, 1)
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) 
{
    (void) index;
    return desc_configuration;
}

char const *string_desc_arr[] =
{
    (const char[]){0x09, 0x04}, // idioma suportado é o inglês
    "UNISAL-2023",
    "Multi-UNISAL",
    "000001"
};

static uint16_t _desc_str[32];

uint16_t const *tud_descriptor_string_cb(uint8_t index , uint16_t langid)
{
    (void) langid;

    uint8_t char_cont;

    if ( index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        char_cont = 1;
    }
    else 
    {
        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
        {
            return NULL;
        }

        const char *tmp = string_desc_arr[index];

        char_cont = strlen(tmp);

        if (char_cont > 31) 
        {
            char_cont = 31;
        }

        for ( uint8_t i = 0; i < char_cont; i++) 
        {
            _desc_str[ i + 1 ] = tmp[i];
        }
    }
    
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * char_cont + 2);

    return _desc_str;
}