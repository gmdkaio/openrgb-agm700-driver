# Hardware Info
- Vendor ID: 0x3938 (Acrox)
- Product ID: 0x1161 (AGM700)

# Protocol Discoveries
The mouse uses a 65-byte Feature Report (Report ID 0x20).
Color bytes are located at offsets 51, 52, and 53 (R, G, B).

- Red payload: `... 14 FF 00 00 00 FF 00 10 ...`
- Blue payload: `... 14 00 00 FF 00 00 FF 10 ...`

# Solution

Integrating with OpenRGB was too complicated, decided to integrate with my matugen config and set up a color change based on my wallpaper. 