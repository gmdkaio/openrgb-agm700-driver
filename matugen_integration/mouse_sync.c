#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hidapi/hidapi.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <HEX_COLOR>\n", argv[0]);
        return 1;
    }

    // Convert hex string (e.g., "ff00aa") to integers
    unsigned int r, g, b;
    if (sscanf(argv[1], "%02x%02x%02x", &r, &g, &b) != 3) {
        fprintf(stderr, "Invalid hex color format.\n");
        return 1;
    }

    if (hid_init() != 0) return 1;

    unsigned char payload[65] = {
        0x20, 0x01, 0x00, 0x02, 0x00, 0x02, 0x04, 0x01, 0x20, 0x00,
        0xff, 0x00, 0x00, 0x01, 0x40, 0x00, 0xff, 0x00, 0x00, 0x01, 
        0x80, 0x00, 0xff, 0x00, 0x00, 0x01, 0x00, 0x01, 0xff, 0x00, 
        0x00, 0x01, 0x40, 0x01, 0xff, 0x00, 0x00, 0x01, 0x01, 0x00, 
        0xff, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x14, 
        (unsigned char)r, (unsigned char)g, (unsigned char)b, 
        (unsigned char)r, (unsigned char)g, (unsigned char)b, 
        0x10, 0x0a, 0x00, 0x00, 0x08, 0x80, 0x00, 0x32, 0x32
    };

    // Target the Acrox AGM700 specific HID interface
    struct hid_device_info *devs = hid_enumerate(0x3938, 0x1161);
    struct hid_device_info *cur = devs;
    hid_device *handle = NULL;

    while (cur) {
        // Interface 1 is the responsible for mouse leds 
        if (cur->interface_number == 1) {
            handle = hid_open_path(cur->path);
            break;
        }
        cur = cur->next;
    }

    if (handle) {
        hid_send_feature_report(handle, payload, sizeof(payload));
        hid_close(handle);
    }

    hid_free_enumeration(devs);
    hid_exit();
    return 0;
}