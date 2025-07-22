package cmd

import (
	"context"
	"crypto/md5"
	"encoding/base64"
	"encoding/json"
	"fmt"

	"github.com/urfave/cli/v3"
	"lora-sensor-cli/pkg/chunker"
	"lora-sensor-cli/pkg/crypto"
	"lora-sensor-cli/pkg/firmware"
	"lora-sensor-cli/pkg/mqtt"
	"lora-sensor-cli/pkg/urlshortener"
)

// UpdateDevicePayload represents the JSON payload structure
type UpdateDevicePayload struct {
	URL       string `json:"u"`
	Version   string `json:"v"`
	MD5Sum    string `json:"m"`
	Signature string `json:"s"`
}

func NewUpdateDeviceCommand() *cli.Command {
	return &cli.Command{
		Name:  "update-device",
		Usage: "Update a device with new firmware",
		Flags: []cli.Flag{
			&cli.StringFlag{
				Name:     "url",
				Usage:    "URL for the firmware",
				Required: true,
				Sources:  cli.EnvVars("FIRMWARE_URL"),
			},
			&cli.StringFlag{
				Name:     "private-key",
				Usage:    "Path to private key file (generated via sodium-native-keygen)",
				Required: true,
				Sources:  cli.EnvVars("PRIVATE_KEY_PATH"),
			},
			&cli.StringFlag{
				Name:     "version",
				Usage:    "Version of the firmware",
				Required: true,
				Sources:  cli.EnvVars("FIRMWARE_VERSION"),
			},
			&cli.StringFlag{
				Name:     "device-name",
				Usage:    "Name of the device to update",
				Required: true,
				Sources:  cli.EnvVars("DEVICE_NAME"),
			},
			&cli.StringFlag{
				Name:     "mqtt-broker",
				Usage:    "MQTT broker address (e.g., tcp://localhost:1883)",
				Required: true,
				Sources:  cli.EnvVars("MQTT_BROKER"),
			},
			&cli.StringFlag{
				Name:    "mqtt-username",
				Usage:   "MQTT username",
				Sources: cli.EnvVars("MQTT_USERNAME"),
			},
			&cli.StringFlag{
				Name:    "mqtt-password",
				Usage:   "MQTT password",
				Sources: cli.EnvVars("MQTT_PASSWORD"),
			},
			&cli.StringFlag{
				Name:    "mqtt-client-id",
				Usage:   "MQTT client ID",
				Value:   "lora-sensor-cli",
				Sources: cli.EnvVars("MQTT_CLIENT_ID"),
			},
		},
		Action: updateDevice,
	}
}

func updateDevice(ctx context.Context, cmd *cli.Command) error {
	// Get command line arguments
	firmwareURL := cmd.String("url")
	privateKeyPath := cmd.String("private-key")
	version := cmd.String("version")
	deviceName := cmd.String("device-name")
	mqttBroker := cmd.String("mqtt-broker")
	mqttUsername := cmd.String("mqtt-username")
	mqttPassword := cmd.String("mqtt-password")
	mqttClientID := cmd.String("mqtt-client-id")

	fmt.Printf("Processing firmware update for device: %s\n", deviceName)
	fmt.Printf("Firmware URL: %s\n", firmwareURL)
	fmt.Printf("Version: %s\n", version)

	// Step 1: Download firmware to calculate MD5
	fmt.Println("Downloading firmware to calculate MD5...")
	firmwareData, err := firmware.Download(firmwareURL)
	if err != nil {
		return fmt.Errorf("failed to download firmware: %w", err)
	}

	// Step 2: Calculate MD5 sum
	fmt.Println("Calculating MD5 sum...")
	md5Hash := md5.Sum(firmwareData)
	md5Sum := fmt.Sprintf("%x", md5Hash)
	fmt.Printf("MD5 sum: %s\n", md5Sum)

	// Step 3: Shorten URL
	fmt.Println("Shortening URL...")
	shortenedURL, err := urlshortener.Shorten(firmwareURL)
	if err != nil {
		return fmt.Errorf("failed to shorten URL: %w", err)
	}
	fmt.Printf("Shortened URL: %s\n", shortenedURL)

	// Step 4: Load private key
	fmt.Println("Loading private key...")
	privateKey, err := crypto.LoadPrivateKey(privateKeyPath)
	if err != nil {
		return fmt.Errorf("failed to load private key: %w", err)
	}

	// Step 5: Create signature data (shortened URL + MD5 sum)
	signatureData := shortenedURL + md5Sum
	fmt.Printf("Signature data: %s\n", signatureData)

	// Step 6: Sign the data
	fmt.Println("Signing data...")
	signature, err := crypto.Sign([]byte(signatureData), privateKey)
	if err != nil {
		return fmt.Errorf("failed to sign data: %w", err)
	}

	// Step 7: Create JSON payload
	payload := UpdateDevicePayload{
		URL:       shortenedURL,
		Version:   version,
		MD5Sum:    md5Sum,
		Signature: base64.StdEncoding.EncodeToString(signature),
	}

	payloadJSON, err := json.Marshal(payload)
	if err != nil {
		return fmt.Errorf("failed to marshal payload: %w", err)
	}

	fmt.Printf("JSON payload: %s\n", string(payloadJSON))

	// Step 8: Chunk the payload
	fmt.Println("Chunking payload...")
	chunks, err := chunker.ChunkPayload(payloadJSON, 51)
	if err != nil {
		return fmt.Errorf("failed to chunk payload: %w", err)
	}

	fmt.Printf("Payload chunked into %d chunks\n", len(chunks))

	// Step 9: Connect to MQTT and publish
	fmt.Println("Connecting to MQTT broker...")
	mqttClient, err := mqtt.NewClient(mqtt.Config{
		Broker:   mqttBroker,
		Username: mqttUsername,
		Password: mqttPassword,
		ClientID: mqttClientID,
	})
	if err != nil {
		return fmt.Errorf("failed to create MQTT client: %w", err)
	}

	if err := mqttClient.Connect(); err != nil {
		return fmt.Errorf("failed to connect to MQTT broker: %w", err)
	}
	defer mqttClient.Disconnect()

	// Step 10: Publish chunks to TTN MQTT topic
	topic := fmt.Sprintf("ttn/soil-conditions/devices/%s/down/push", deviceName)
	fmt.Printf("Publishing to topic: %s\n", topic)
	fmt.Printf("Publishing %d chunks as single TTN message\n", len(chunks))

	if err := mqttClient.PublishChunks(topic, chunks); err != nil {
		return fmt.Errorf("failed to publish chunks: %w", err)
	}

	fmt.Println("Firmware update payload published successfully!")
	return nil
}
