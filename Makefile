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

.PHONY: build-cli
build-cli:
	cd cli && go build -o lora-sensor-cli .

.PHONY: lint-cli
lint-cli:
	cd cli && golangci-lint run
