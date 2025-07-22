# CREATED BY VIM-PIO
all:
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
.PHONY: release
release:
	platformio -f -c vim run -e ttgo-lora32-v1-release

# Build and immediately upload the release firmware
.PHONY: upload-release
upload-release: release
	platformio -f -c vim run -e ttgo-lora32-v1-release --target upload

.PHONY: test
test:
	platformio test -e native

.PHONY: test-cli
test-cli:
	cd cli && go test ./... -v

.PHONY: build-cli
build-cli:
	cd cli && go build -o lora-sensor-cli .

.PHONY: lint-cli
lint-cli:
	cd cli && golangci-lint run
