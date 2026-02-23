#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

int main() {
    int vendor_id = 0x3938;
    int product_id = 0x1161;

    if (hid_init() != 0) {
        printf("Failed to initialize hidapi.\n");
        return 1;
    }

    // Red payload for the AGM700
    unsigned char payload[65] = {
        0x20, 0x01, 0x00, 0x02, 0x00, 0x02, 0x04, 0x01, 0x20, 0x00,
        0xff, 0x00, 0x00, 0x01, 0x40, 0x00, 
        0xff, 0x00, 0x00, 0x01, 0x80, 0x00, 
        0xff, 0x00, 0x00, 0x01, 0x00, 0x01, 
        0xff, 0x00, 0x00, 0x01, 0x40, 0x01, 
        0xff, 0x00, 0x00, 0x01, 0x01, 0x00, 
        0xff, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x14, 
        0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x10, 0x0a, 0x00, 0x00, 
        0x08, 0x80, 0x00, 0x32, 0x32
    };

    // Find all devices matching the AGM700
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(vendor_id, product_id);
    cur_dev = devs;

    int success = 0;

    printf("Hunting for the AGM700 RGB Interface...\n");

    // Loop through every interface the mouse has
    while (cur_dev) {
        printf("Trying Interface %d...\n", cur_dev->interface_number);
        
        hid_device *handle = hid_open_path(cur_dev->path);
        if (handle) {
            // Send the payload
            int res = hid_send_feature_report(handle, payload, sizeof(payload));
            
            if (res >= 0) {
                printf(" -> BOOM! Sent %d bytes to Interface %d.\n", res, cur_dev->interface_number);
                success = 1;
            } else {
                printf(" -> Ignored by this interface.\n");
            }
            hid_close(handle);
        } else {
            printf(" -> Could not open this interface (Permission denied?)\n");
        }
        
        cur_dev = cur_dev->next; // Move to the next interface
    }

    hid_free_enumeration(devs);
    hid_exit();
    
    if (!success) {
        printf("\nFailed to push the color to any interface. Did you use sudo?\n");
    }
    
    return 0;
}