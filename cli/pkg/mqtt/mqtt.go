package mqtt

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

// Config represents MQTT client configuration
type Config struct {
	Broker   string
	Username string
	Password string
	ClientID string
}

// Client wraps the MQTT client
type Client struct {
	client mqtt.Client
}

// TTNDownlink represents a single TTN downlink message
type TTNDownlink struct {
	FPort      int    `json:"f_port"`
	FRMPayload string `json:"frm_payload"`
	Priority   string `json:"priority"`
}

// TTNMessage represents a TTN MQTT message structure with chunked downlinks
type TTNMessage struct {
	Downlinks []TTNDownlink `json:"downlinks"`
}

// NewClient creates a new MQTT client
func NewClient(config Config) (*Client, error) {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(config.Broker)
	opts.SetClientID(config.ClientID)
	if config.Username != "" {
		opts.SetUsername(config.Username)
	}
	if config.Password != "" {
		opts.SetPassword(config.Password)
	}
	opts.SetConnectTimeout(10 * time.Second)
	opts.SetWriteTimeout(10 * time.Second)
	opts.SetKeepAlive(60 * time.Second)
	opts.SetDefaultPublishHandler(func(client mqtt.Client, msg mqtt.Message) {
		fmt.Printf("Received message: %s from topic: %s\n", msg.Payload(), msg.Topic())
	})

	client := mqtt.NewClient(opts)
	return &Client{client: client}, nil
}

// Connect connects to the MQTT broker
func (c *Client) Connect() error {
	token := c.client.Connect()
	if token.Wait() && token.Error() != nil {
		return fmt.Errorf("failed to connect to MQTT broker: %w", token.Error())
	}
	return nil
}

// Disconnect disconnects from the MQTT broker
func (c *Client) Disconnect() {
	c.client.Disconnect(250)
}

// PublishChunks publishes multiple chunks as a single TTN MQTT message with downlinks array
func (c *Client) PublishChunks(topic string, chunks [][]byte) error {
	// Create TTN message structure with multiple downlinks
	message := TTNMessage{
		Downlinks: make([]TTNDownlink, len(chunks)),
	}

	// Create downlink for each chunk with incrementing f_port
	for i, chunk := range chunks {
		message.Downlinks[i] = TTNDownlink{
			FPort:      i + 1, // f_port starts at 1 and increments
			FRMPayload: base64.StdEncoding.EncodeToString(chunk),
			Priority:   "NORMAL",
		}
	}

	// Marshal to JSON
	payload, err := json.Marshal(message)
	if err != nil {
		return fmt.Errorf("failed to marshal TTN message: %w", err)
	}

	// Publish the message
	token := c.client.Publish(topic, 0, false, payload)
	if token.Wait() && token.Error() != nil {
		return fmt.Errorf("failed to publish message: %w", token.Error())
	}

	return nil
}

// Publish publishes a single chunk as a TTN MQTT message (deprecated, use PublishChunks)
func (c *Client) Publish(topic string, chunk []byte) error {
	return c.PublishChunks(topic, [][]byte{chunk})
}

// IsConnected returns true if the client is connected
func (c *Client) IsConnected() bool {
	return c.client.IsConnected()
}
