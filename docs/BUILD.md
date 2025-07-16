# Build Documentation

This document provides instructions and tips for building the firmware of the LoRa Soil Moisture Sensor project.

## Build Environments

- **ttgo-lora32-v1-release**: Production build with optimizations for size.
- **ttgo-lora32-v1-debug**: Debug build with additional logging and diagnostic information.

## Build Size Optimization Tips

1. **String Storage**: Store string constants in PROGMEM to reduce SRAM usage.
2. **Conditional Compilation**: Use DEBUG flags to include debug code only in debug builds.
3. **Compiler Flags**: Use `-Os`, `-ffunction-sections`, `-fdata-sections`, `-Wl,--gc-sections` to optimize for size.

## Current Build Sizes

- **Release Build**:
  - RAM: 16.8% (used 55,084 bytes from 327,680 bytes)
  - Flash: 92.4% (used 1,211,449 bytes from 1,310,720 bytes)

- **Debug Build**:
  - RAM: 17.3% (used 56,640 bytes from 327,680 bytes)
  - Flash: 101.4% (exceeds size limit)

## Size Reduction

- **Before Optimization**:
  - Flash: 99.3% (used 1,302,037 bytes)

- **After Optimization**:
  - Flash: 92.4% (used 1,211,449 bytes)

- **Total Reduction**: 90,588 bytes (~14 KB)

## Building the Firmware

To build the firmware, use the following commands:

For release:
```bash
pio run --environment ttgo-lora32-v1-release
```

For debug:
```bash
pio run --environment ttgo-lora32-v1-debug
```

## Skipping Size Checks

For development purposes, you may skip the size check:
```
extra_scripts = pre:skip_size_check.py
```

This concludes the build documentation.
