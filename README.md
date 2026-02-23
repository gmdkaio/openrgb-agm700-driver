Acrox AGM700 Linux RGB Sync

A simple suite of C tools to control the Acrox AGM700 mouse (VID: 3938, PID: 1161) and sync it with Matugen and OpenRGB on Linux.
1. Installation

Ensure you have hidapi installed on your system.
Bash

# Compile both tools
gcc -o mouse_gui mouse_gui.c $(pkg-config --cflags --libs hidapi-hidraw)
gcc -o mouse_sync mouse_sync.c $(pkg-config --cflags --libs hidapi-hidraw)

# Move the sync tool to your path for automation
sudo mv mouse_sync /usr/local/bin/

2. The Tools
Continuous GUI (mouse_gui.c)

Opens a Zenity color picker in a loop. Every color selected updates the mouse instantly.

    Usage: ./mouse_gui

CLI Sync (mouse_sync.c)

A lightweight binary for scripts.

    Usage: mouse_sync <HEX_COLOR> (e.g., mouse_sync ff0055)

3. Matugen & OpenRGB Automation

Integrate the mouse into your Niri / Matugen workflow by updating your templates.

Template: ~/.config/matugen/templates/openrgb-sync.sh
Bash

#!/bin/bash
HEX="{{colors.source_color.default.hex_stripped}}"

# Sync Motherboard/RAM
openrgb -c $HEX

# Sync Acrox Mouse
mouse_sync $HEX

Config Hook: ~/.config/matugen/config.toml
Ini, TOML

[templates.openrgb]
input_path = "~/.config/matugen/templates/openrgb-sync.sh"
output_path = "~/.config/matugen/scripts/openrgb-sync.sh"
post_hook = "bash ~/.config/matugen/scripts/openrgb-sync.sh"

4. Permissions (Udev Rule)

Run this once so the tools can access the mouse without sudo.

Create /etc/udev/rules.d/99-acrox-mouse.rules:
Plaintext

KERNEL=="hidraw*", ATTRS{idVendor}=="3938", ATTRS{idProduct}=="1161", MODE="0666"

Apply with: sudo udevadm control --reload-rules && sudo udevadm trigger