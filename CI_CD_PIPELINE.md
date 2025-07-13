# CI/CD Pipeline Documentation

## Overview

This project uses a comprehensive GitHub Actions CI/CD pipeline with three distinct stages designed for different development scenarios. The pipeline is defined in `.github/workflows/ci-cd.yml`.

## Pipeline Stages

### 1. Branch Development Pipeline
**Trigger**: Pull requests and pushes to any branch (except main)
**Purpose**: Development and feature branch validation

**Steps**:
- ✅ Unit Tests (simplified native tests)
- ✅ Compilation
- ✅ Code Coverage Generation
- ✅ SonarQube Analysis

**When it runs**:
- On every pull request (opened, synchronized, reopened)
- On pushes to any branch except `main`

### 2. Main Branch Pipeline
**Trigger**: Pushes to `main` branch
**Purpose**: Production branch validation

**Steps**:
- ✅ Unit Tests (simplified native tests)
- ✅ Compilation
- ✅ Code Coverage Generation
- ✅ SonarQube Analysis

**When it runs**:
- On every push to the `main` branch

### 3. Release Pipeline
**Trigger**: Tag creation with float number format
**Purpose**: Production release with artifacts

**Steps**:
- ✅ Tag Format Validation
- ✅ Unit Tests
- ✅ Compilation with Firmware Version
- ✅ Artifact Creation
- ✅ GitHub Release Creation

**When it runs**:
- On tag creation that matches the pattern `[0-9]+.[0-9]+` (float numbers only)

## Tag Validation Rules

### ✅ Valid Tag Formats (Float Numbers Only)
- `1.0` - Basic float number
- `1.1` - Float with decimal
- `2.0` - Integer with decimal
- `10.2` - Multi-digit float

### ❌ Invalid Tag Formats
- `v1.0` - Cannot start with 'v'
- `1.0.0` - Cannot use semantic versioning (3 parts)
- `1.0.1` - Cannot use semantic versioning (3 parts)
- `1` - Must have decimal point
- `1.` - Must have digits after decimal
- `version1.0` - Cannot contain letters

### Tag Validation Logic
The pipeline will **FAIL** if:
1. Tag starts with 'v' (e.g., `v1.0`)
2. Tag doesn't match the float number pattern (X.Y format only)
3. Tag contains invalid characters
4. Tag uses semantic versioning (X.Y.Z format)

## Firmware Version Integration

When a valid tag is created, the pipeline automatically:
1. Extracts the version number from the tag
2. Passes it to PlatformIO as a build flag: `-D FIRMWARE_VERSION=<tag>`
3. Embeds the version in the compiled firmware

**Example**:
```bash
# Tag: 1.1
# Build flag: -D FIRMWARE_VERSION=1.1
```

## Release Artifacts

Each successful release creates the following artifacts:

### Binary Files
- `firmware-<version>.bin` - ESP32 binary firmware file
- `firmware-<version>.elf` - ELF debug file
- `partitions-<version>.bin` - ESP32 partition table

### Metadata
- `build-info-<version>.json` - Build information and metadata

### GitHub Release
- Automatic GitHub release with all artifacts attached
- Release notes with build information
- Downloadable firmware files

## Usage Examples

### Creating a Development Branch
```bash
git checkout -b feature/new-feature
# Make changes
git push origin feature/new-feature
# Creates a pull request
# Pipeline runs automatically
```

### Merging to Main
```bash
# Merge pull request to main
# Main pipeline runs automatically
```

### Creating a Release (Float Numbers Only)
```bash
# Create and push a valid tag
git tag 1.0
git push origin 1.0

# Or create next release
git tag 1.1
git push origin 1.1
```

### Invalid Tag (Will Fail)
```bash
# These will cause the pipeline to fail
git tag v1.0
git push origin v1.0

git tag 1.0.0
git push origin 1.0.0
```

## Pipeline Configuration

### Environment Variables
- `BUILD_WRAPPER_OUT_DIR`: SonarQube build wrapper output directory
- `PLATFORMIO_CACHE_DIR`: PlatformIO cache directory

### Secrets Required
- `SONAR_TOKEN`: SonarQube authentication token
- `GITHUB_TOKEN`: Automatically provided by GitHub

### Caching Strategy
- PlatformIO cache is cached between runs
- Cache key includes platformio.ini hash for invalidation
- Fallback to partial cache if exact match not found

## Unit Tests

The pipeline runs simplified unit tests that focus on core logic:
- `test_ota_simple` - OTA JSON parsing and validation
- `test_wifi_simple` - WiFi settings management
- `test_utils_simple` - Utility functions
- `test_constants_simple` - Constant validation

These tests run in the native environment and provide fast feedback without complex platform stubbing.

## Coverage Reporting

Code coverage is generated using `gcovr` with:
- SonarQube-compatible XML format
- Exclusion of test files and dependencies
- Integration with SonarQube analysis

## SonarQube Integration

SonarQube analysis includes:
- Compilation database for accurate analysis
- Coverage reports for quality metrics
- Code quality and security analysis

## Troubleshooting

### Pipeline Failures

1. **Tag Validation Failed**
   - Check tag format (must be float number X.Y, no 'v' prefix)
   - Ensure tag follows float number format (not semantic versioning)

2. **Unit Tests Failed**
   - Check test output for specific failures
   - Verify test logic in simplified test files

3. **Compilation Failed**
   - Check PlatformIO configuration
   - Verify dependencies in platformio.ini

4. **SonarQube Failed**
   - Verify SONAR_TOKEN secret is configured
   - Check SonarQube project configuration

### Common Issues

1. **Cache Issues**
   - Clear cache by updating platformio.ini
   - Check cache key configuration

2. **Permission Issues**
   - Ensure repository has proper permissions
   - Check GitHub token permissions

## Migration from Legacy Workflow

The old workflow in `build.yml` has been deprecated. The new pipeline provides:
- ✅ Better separation of concerns
- ✅ Tag validation and release automation
- ✅ Improved caching strategy
- ✅ Comprehensive artifact management
- ✅ Better error handling and reporting

## Best Practices

1. **Tag Naming**
   - Use float numbers (X.Y format)
   - Never prefix with 'v'
   - Never use semantic versioning (X.Y.Z)

2. **Branch Management**
   - Use feature branches for development
   - Merge to main only after review
   - Tag releases from main branch

3. **Testing**
   - Ensure all tests pass before merging
   - Use simplified tests for fast feedback
   - Test on hardware for integration issues

4. **Release Management**
   - Create releases from main branch
   - Use descriptive release notes
   - Archive old releases as needed 