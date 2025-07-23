# CREATED BY VIM-PIO
all: release
	platformio -f -c vim run

upload:
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean

program:
	platformio -f -c vim run --target program

uploadfs:
	platformio -f -c vim run --target uploadfs

# Build the optimized release firmware
.PHONY: release minimal test
release:  ## Build main release env
	platformio run -e ttgo-lora32-v1
minimal:
	platformio run -e minimal

# Test target that calls minimal build for CI readability
test: minimal  ## Build minimal env (alias for CI)

# Memory testing targets for OTA crash harness
.PHONY: memory-testing upload-memory-testing monitor-memory-testing
memory-testing:  ## Build memory testing harness
	platformio run -e ota_crash_harness

upload-memory-testing:  ## Upload memory testing harness to device
	platformio run -e ota_crash_harness --target upload

monitor-memory-testing:  ## Monitor serial output from memory testing harness
	platformio device monitor -e ota_crash_harness

configure-wifi:  ## Configure WiFi credentials for memory testing
	@echo "Please edit src/ota_crash_harness.cpp and update the following lines:"
	@echo "  const char* ssid = \"YOUR_WIFI_SSID_HERE\";"
	@echo "  const char* password = \"YOUR_WIFI_PASSWORD_HERE\";"
	@echo ""
	@echo "Current configuration:"
	@grep -n "const char\* ssid" src/ota_crash_harness.cpp || echo "File not found"
	@grep -n "const char\* password" src/ota_crash_harness.cpp || echo "File not found"


.PHONY: build-cli
build-cli:
	cd cli && go build -o lora-sensor-cli .

.PHONY: lint-cli
lint-cli:
	cd cli && golangci-lint run
