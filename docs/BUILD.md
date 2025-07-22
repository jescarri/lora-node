# Build Documentation

This document provides instructions and tips for building the firmware of the LoRa Soil Moisture Sensor project.

## Build Environment

- **ttgo-lora32-v1-release**: Production build with optimizations for size.

## Build Size Optimization Tips

1. **String Storage**: Store string constants in PROGMEM to reduce SRAM usage.
2. **Conditional Compilation**: Use DEBUG flags during development for diagnostic information.
3. **Compiler Flags**: Use `-Os`, `-ffunction-sections`, `-fdata-sections`, `-Wl,--gc-sections` to optimize for size.

## Current Build Size

- **Release Build**:
  - RAM: 16.8% (used 55,084 bytes from 327,680 bytes)
  - Flash: 92.4% (used 1,211,449 bytes from 1,310,720 bytes)

## Size Reduction

- **Before Optimization**:
  - Flash: 99.3% (used 1,302,037 bytes)

- **After Optimization**:
  - Flash: 92.4% (used 1,211,449 bytes)

- **Total Reduction**: 90,588 bytes (~14 KB)

## Building the Firmware

To build the firmware, use the following command:

```bash
pio run --environment ttgo-lora32-v1-release
```

## Skipping Size Checks

For development purposes, you may skip the size check:
```
extra_scripts = pre:skip_size_check.py
```

This concludes the build documentation.
