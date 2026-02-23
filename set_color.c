#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hidapi/hidapi.h>

// New hardware function to keep a cleaner code 
void set_mouse_color(unsigned char r, unsigned char g, unsigned char b) {
    int vendor_id = 0x3938;
    int product_id = 0x1161;

    if (hid_init() != 0) return;

    unsigned char payload[65] = {
        0x20, 0x01, 0x00, 0x02, 0x00, 0x02, 0x04, 0x01, 0x20, 0x00,
        0xff, 0x00, 0x00, 0x01, 0x40, 0x00, 0xff, 0x00, 0x00, 0x01, 
        0x80, 0x00, 0xff, 0x00, 0x00, 0x01, 0x00, 0x01, 0xff, 0x00, 
        0x00, 0x01, 0x40, 0x01, 0xff, 0x00, 0x00, 0x01, 0x01, 0x00, 
        0xff, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x14, 
        r, g, b, // ZONE 1
        r, g, b, // ZONE 2 
        0x10, 0x0a, 0x00, 0x00, 0x08, 0x80, 0x00, 0x32, 0x32
    };

    struct hid_device_info *devs = hid_enumerate(vendor_id, product_id);
    struct hid_device_info *cur_dev = devs;

    while (cur_dev) {
        hid_device *handle = hid_open_path(cur_dev->path);
        if (handle) {
            hid_send_feature_report(handle, payload, sizeof(payload));
            hid_close(handle);
        }
        cur_dev = cur_dev->next;
    }

    hid_free_enumeration(devs);
    hid_exit();
}

int main() {
    char output[128];
    int r = 0, g = 0, b = 0;

    printf("Spawning continuous color picker GUI...\n");
    printf("Click 'Cancel' or close the window to exit.\n");

    // Infinite loop to test as many colors as I want 
    while (1) {
        FILE *fp = popen("zenity --color-selection 2>/dev/null", "r");
        if (fp == NULL) {
            printf("Failed to launch the picker.\n");
            return 1;
        }

        // If fgets grabs text, then select was clicked 
        if (fgets(output, sizeof(output), fp) != NULL) {
            
            if (sscanf(output, "rgb(%d,%d,%d)", &r, &g, &b) == 3 || 
                sscanf(output, "rgba(%d,%d,%d", &r, &g, &b) == 3) {
            } else if (output[0] == '#') {
                int hex;
                sscanf(output, "#%06x", &hex);
                r = (hex >> 16) & 0xFF;
                g = (hex >> 8) & 0xFF;
                b = hex & 0xFF;
            } else {
                printf("Could not parse color format: %s\n", output);
                pclose(fp);
                break; // Safety breakout
            }

            printf("Injecting RGB: %d %d %d\n", r, g, b);
            set_mouse_color((unsigned char)r, (unsigned char)g, (unsigned char)b);
            pclose(fp);
            
        } else {
            // If fgets fails, then cancel was clicked or the window was closed
            printf("Picker closed. Exiting...\n");
            pclose(fp);
            break; // Break the loop and end the program
        }
    }

    return 0;
}

