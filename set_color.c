#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

int main(int argc, char *argv[]) {
    // Check if rgb values were provided
    if (argc != 4) {
        printf("Usage: sudo ./set_color <R> <G> <B>\n");
        printf("Example (Pure Cyan): sudo ./set_color 0 255 255\n");
        return 1;
    }

    // Convert input to integers and then to unsigned char
    unsigned char r = (unsigned char)atoi(argv[1]);
    unsigned char g = (unsigned char)atoi(argv[2]);
    unsigned char b = (unsigned char)atoi(argv[3]);

    int vendor_id = 0x3938;
    int product_id = 0x1161;

    if (hid_init() != 0) {
        printf("Failed to initialize hidapi.\n");
        return 1;
    }

    // Dynamic payload with the RGB values from the input
    unsigned char payload[65] = {
        0x20, 0x01, 0x00, 0x02, 0x00, 0x02, 0x04, 0x01, 0x20, 0x00,
        0xff, 0x00, 0x00, 0x01, 0x40, 0x00, 
        0xff, 0x00, 0x00, 0x01, 0x80, 0x00, 
        0xff, 0x00, 0x00, 0x01, 0x00, 0x01, 
        0xff, 0x00, 0x00, 0x01, 0x40, 0x01, 
        0xff, 0x00, 0x00, 0x01, 0x01, 0x00, 
        0xff, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x14, 
        
        r, g, b, // <-- ZONE 1 (Main Body) dynamically set
        r, g, b, // <-- ZONE 2 (Scroll Wheel) dynamically set (Hate that theres two. Makes my life harder)
        
        0x10, 0x0a, 0x00, 0x00, 0x08, 0x80, 0x00, 0x32, 0x32
    };

    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(vendor_id, product_id);
    cur_dev = devs;

    int success = 0;

    while (cur_dev) {
        hid_device *handle = hid_open_path(cur_dev->path);
        if (handle) {
            int res = hid_send_feature_report(handle, payload, sizeof(payload));
            if (res >= 0) {
                printf("Successfully applied RGB(%d, %d, %d) to Interface %d\n", r, g, b, cur_dev->interface_number);
                success = 1;
            }
            hid_close(handle);
        }
        cur_dev = cur_dev->next;
    }

    hid_free_enumeration(devs);
    hid_exit();
    
    if (!success) {
        printf("Failed to communicate with the AGM700.\n");
    }
    
    return 0;
}