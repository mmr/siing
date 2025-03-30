#ifndef USB_DEVICE_H
#define USB_DEVICE_H

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

// Initialize USB subsystem
bool usb_device_init(void);

// Scan for connected USB devices
void usb_device_scan(void);

// Close USB subsystem
void usb_device_close(void);

// Check if a specific device is connected
bool usb_device_is_connected(int device_id);

// Get device input data
bool usb_device_get_input(int device_id, void *data, int size);

#endif // USB_DEVICE_H 