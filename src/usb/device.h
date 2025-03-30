#ifndef _USB_DEVICE_H_
#define _USB_DEVICE_H_

#include <gccore.h>
#include <ogc/usb.h>

// Device types
typedef enum {
    DEVICE_TYPE_NONE = 0,
    DEVICE_TYPE_GUITAR,
    DEVICE_TYPE_DRUMS,
    DEVICE_TYPE_MICROPHONE
} device_type_t;

// Device structure
typedef struct {
    device_type_t type;
    u8 endpoint;
    u8 interface;
    u16 vendor_id;
    u16 product_id;
    void *data;
} usb_device_t;

// Function declarations
int usb_init_device(usb_device_t *device);
int usb_detect_devices(void);
int usb_handle_input(usb_device_t *device);
int usb_cleanup_device(usb_device_t *device);

// Device-specific functions
int guitar_init(usb_device_t *device);
int drums_init(usb_device_t *device);
int microphone_init(usb_device_t *device);

#endif /* _USB_DEVICE_H_ */ 