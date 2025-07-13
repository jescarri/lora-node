# Firmware Version System

## Overview

The firmware version system has been implemented to dynamically handle version information at both compile-time and runtime. The system integrates with the CI/CD pipeline to automatically embed version information during releases.

## Architecture

### Files Created/Modified

1. **`src/config.hpp`** - Central configuration with version constants
2. **`src/version.hpp`** - Version management interface
3. **`src/version.cpp`** - Version management implementation
4. **`src/main.cpp`** - Startup version display
5. **`src/ota.hpp`** - OTA version reporting
6. **`src/ota.cpp`** - OTA version integration

## Version System Components

### 1. Configuration (`config.hpp`)

```cpp
// Firmware Version -----------------------------------------------------------
// The FIRMWARE_VERSION is set by the CI/CD pipeline during compilation
// Format: X.Y (e.g., 1.0, 1.1, 2.0)
#ifdef FIRMWARE_VERSION
    constexpr float FirmwareVersionFloat = FIRMWARE_VERSION;
#else
    constexpr float FirmwareVersionFloat = 0.0f; // Development build
#endif
```

### 2. Version Interface (`version.hpp`)

```cpp
namespace version {
    // Get firmware version as float
    float getFirmwareVersionFloat();
    
    // Get firmware version as string
    const char* getFirmwareVersion();
    
    // Print version information to Serial
    void printFirmwareVersion();
    
    // Check if development build
    bool isDevelopmentBuild();
    
    // Get build information
    const char* getBuildInfo();
}
```

### 3. Version Implementation (`version.cpp`)

```cpp
const char* getFirmwareVersion() {
    static char versionString[16];
    
    if (isDevelopmentBuild()) {
        return "dev";
    } else {
        // Convert float to string (e.g., 1.0 -> "1.0")
        snprintf(versionString, sizeof(versionString), "%.1f", getFirmwareVersionFloat());
        return versionString;
    }
}
```

## CI/CD Integration

### Build Flag Injection

When a valid tag is created (e.g., `1.0`), the CI/CD pipeline:

1. **Validates tag format** (float numbers only)
2. **Passes version to PlatformIO**: `-D FIRMWARE_VERSION=1.0`
3. **Compiles with version embedded**
4. **Creates release artifacts**

### Example Flow

```bash
# Create release tag
git tag 1.0
git push origin 1.0

# CI/CD pipeline executes:
pio run --build-flag="-D FIRMWARE_VERSION=1.0"

# Results in:
# - config::FirmwareVersionFloat = 1.0f
# - version::getFirmwareVersion() = "1.0"
# - version::getFirmwareVersionFloat() = 1.0f
```

## Startup Display

The firmware version is automatically displayed at startup in `main.cpp`:

```cpp
void setup() {
    // ... initialization code ...
    Serial.begin(115200);
    
    // Print firmware version information at startup
    version::printFirmwareVersion();
    
    // ... rest of setup ...
}
```

### Sample Output

```
=== Firmware Information ===
Version: 1.0
Build Type: Release
Build Info: Release v1.0
===========================
```

## Version Formats

### ✅ Valid Versions (CI/CD Pipeline)

| Tag | Float Value | String Value | Build Type |
|-----|-------------|--------------|------------|
| `1.0` | `1.0f` | `"1.0"` | Release |
| `1.1` | `1.1f` | `"1.1"` | Release |
| `2.0` | `2.0f` | `"2.0"` | Release |

### ❌ Invalid Versions (Pipeline Fails)

| Tag | Reason |
|-----|--------|
| `v1.0` | Starts with 'v' |
| `1.0.0` | Semantic versioning |
| `1` | No decimal point |

### Development Builds

When no `FIRMWARE_VERSION` is defined:
- Float value: `0.0f`
- String value: `"dev"`
- Build type: Development

## OTA Integration

### Version Reporting

The OTA system reports the current firmware version via LoRaWAN:

```cpp
void reportFirmwareVersion(CayenneLPP& lpp) {
    float version = version::getFirmwareVersionFloat();
    lpp.addGenericSensor(10, version);
}
```

### Version Comparison

During OTA updates, the system can compare versions:

```cpp
// In OTA message parsing
if (updateInfo.version.toFloat() > version::getFirmwareVersionFloat()) {
    // Newer version available
    downloadAndInstallFirmware(updateInfo);
}
```

## Usage Examples

### In Your Code

```cpp
#include "version.hpp"

void someFunction() {
    // Get version as float
    float version = version::getFirmwareVersionFloat();
    
    // Get version as string
    const char* versionStr = version::getFirmwareVersion();
    
    // Check build type
    if (version::isDevelopmentBuild()) {
        Serial.println("Development build");
    } else {
        Serial.printf("Release version: %s\n", versionStr);
    }
}
```

### Version Comparison

```cpp
// Compare versions
float currentVersion = version::getFirmwareVersionFloat();
float newVersion = 1.1f;

if (newVersion > currentVersion) {
    Serial.println("Newer version available");
}
```

## Benefits

### 1. **Automatic Version Management**
- No manual version updates required
- Version automatically embedded during CI/CD
- Consistent versioning across builds

### 2. **Runtime Flexibility**
- Version available as both float and string
- Development vs release build detection
- Easy version comparison

### 3. **Integration Ready**
- OTA version reporting
- Startup version display
- LoRaWAN integration

### 4. **Type Safety**
- Compile-time version constants
- Runtime string conversion
- Proper error handling

## Testing

The version system is tested through:

1. **Unit Tests**: `test_constants_simple` validates version constants
2. **Build Tests**: Compilation with and without version flags
3. **Integration Tests**: OTA and LoRaWAN version reporting

## Migration Notes

### From Previous Version System

If you were using a hardcoded version:

```cpp
// Old way
#define FIRMWARE_VERSION 100
constexpr float FIRMWARE_VERSION_FLOAT = FIRMWARE_VERSION;

// New way
float version = version::getFirmwareVersionFloat();
const char* versionStr = version::getFirmwareVersion();
```

### Benefits of New System

1. **Dynamic**: Version set by CI/CD pipeline
2. **Flexible**: Both float and string access
3. **Safe**: Development build detection
4. **Integrated**: Startup display and OTA reporting

## Troubleshooting

### Common Issues

1. **Version shows as "dev"**
   - Check if `FIRMWARE_VERSION` is defined
   - Verify CI/CD pipeline is passing the flag correctly

2. **Version comparison issues**
   - Use `version::getFirmwareVersionFloat()` for numeric comparisons
   - Use `version::getFirmwareVersion()` for string display

3. **Build failures**
   - Ensure tag format is correct (float numbers only)
   - Check CI/CD pipeline validation

### Debug Information

Enable debug output by adding:

```cpp
Serial.printf("Debug - Float version: %.1f\n", version::getFirmwareVersionFloat());
Serial.printf("Debug - String version: %s\n", version::getFirmwareVersion());
Serial.printf("Debug - Build type: %s\n", version::isDevelopmentBuild() ? "Development" : "Release");
```

## Future Enhancements

### Potential Improvements

1. **Version History**: Track version changes in preferences
2. **Rollback Support**: Store previous version for rollback
3. **Version Validation**: Validate version format at runtime
4. **Build Timestamp**: Include build date/time in version info

The version system is now fully integrated and ready for production use! 🎯 