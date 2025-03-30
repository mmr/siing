#include "device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of supported USB devices
#define MAX_USB_DEVICES 4
#define MAX_USB_DEVICE_ENTRIES 8

// Device state structure
typedef struct {
    bool connected;
    s32 fd;
    u16 vid;
    u16 pid;
} usb_device_state_t;

// Global device states
static usb_device_state_t devices[MAX_USB_DEVICES];

bool usb_device_init(void) {
    // Initialize USB subsystem
    if (USB_Initialize() < 0) {
        return false;
    }
    
    // Clear device states
    memset(devices, 0, sizeof(devices));
    return true;
}

void usb_device_scan(void) {
    // Get list of connected devices
    usb_device_entry entries[MAX_USB_DEVICE_ENTRIES];
    u8 count = 0;
    
    if (USB_GetDeviceList(entries, MAX_USB_DEVICE_ENTRIES, 0, &count) < 0) {
        return;
    }
    
    // Update device states
    for (int i = 0; i < MAX_USB_DEVICES && i < count; i++) {
        usb_device_state_t *dev = &devices[i];
        usb_device_entry *entry = &entries[i];
        
        // Check if device is already connected
        if (!dev->connected) {
            // Try to open device
            s32 fd;
            if (USB_OpenDevice(entry->device_id, entry->vid, entry->pid, &fd) >= 0) {
                dev->connected = true;
                dev->fd = fd;
                dev->vid = entry->vid;
                dev->pid = entry->pid;
            }
        }
    }
}

void usb_device_close(void) {
    // Close all open devices
    for (int i = 0; i < MAX_USB_DEVICES; i++) {
        usb_device_state_t *dev = &devices[i];
        if (dev->connected) {
            USB_CloseDevice(&dev->fd);
            dev->connected = false;
        }
    }
    
    // Shutdown USB subsystem
    USB_Deinitialize();
}

bool usb_device_is_connected(int device_id) {
    if (device_id < 0 || device_id >= MAX_USB_DEVICES) {
        return false;
    }
    return devices[device_id].connected;
}

bool usb_device_get_input(int device_id, void *data, int size) {
    if (!data || size <= 0 || device_id < 0 || device_id >= MAX_USB_DEVICES) {
        return false;
    }
    
    usb_device_state_t *dev = &devices[device_id];
    if (!dev->connected) {
        return false;
    }
    
    // Read data from device
    return USB_ReadIntrMsg(dev->fd, 0x81, size, data) >= 0;
} 