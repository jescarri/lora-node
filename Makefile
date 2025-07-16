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
