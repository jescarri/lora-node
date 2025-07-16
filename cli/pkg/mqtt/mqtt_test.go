package mqtt

import (
	"encoding/base64"
	"encoding/json"
	"testing"
)

func TestNewClient(t *testing.T) {
	config := Config{
		Broker:   "tcp://localhost:1883",
		Username: "test_user",
		Password: "test_pass",
		ClientID: "test_client",
	}

	client, err := NewClient(config)
	if err != nil {
		t.Fatalf("Failed to create client: %v", err)
	}

	if client == nil {
		t.Fatal("Client is nil")
	}

	if client.client == nil {
		t.Fatal("Internal client is nil")
	}
}

func TestTTNMessageStructure(t *testing.T) {
	// Test the TTN message structure
	message := TTNMessage{}
	message.DownlinkMessage.FPort = 1
	message.DownlinkMessage.FRMPayload = base64.StdEncoding.EncodeToString([]byte("test data"))
	message.DownlinkMessage.Priority = "NORMAL"

	// Marshal to JSON
	payload, err := json.Marshal(message)
	if err != nil {
		t.Fatalf("Failed to marshal TTN message: %v", err)
	}

	// Unmarshal back to verify structure
	var unmarshaled TTNMessage
	if err := json.Unmarshal(payload, &unmarshaled); err != nil {
		t.Fatalf("Failed to unmarshal TTN message: %v", err)
	}

	if unmarshaled.DownlinkMessage.FPort != 1 {
		t.Errorf("Expected FPort 1, got %d", unmarshaled.DownlinkMessage.FPort)
	}

	if unmarshaled.DownlinkMessage.Priority != "NORMAL" {
		t.Errorf("Expected Priority NORMAL, got %s", unmarshaled.DownlinkMessage.Priority)
	}

	// Decode the FRMPayload
	decodedData, err := base64.StdEncoding.DecodeString(unmarshaled.DownlinkMessage.FRMPayload)
	if err != nil {
		t.Fatalf("Failed to decode FRMPayload: %v", err)
	}

	if string(decodedData) != "test data" {
		t.Errorf("Expected 'test data', got %s", string(decodedData))
	}
}

func TestTTNMessageJSON(t *testing.T) {
	// Test that the JSON structure matches expected format
	message := TTNMessage{}
	message.DownlinkMessage.FPort = 1
	message.DownlinkMessage.FRMPayload = "dGVzdA=="
	message.DownlinkMessage.Priority = "NORMAL"

	payload, err := json.Marshal(message)
	if err != nil {
		t.Fatalf("Failed to marshal TTN message: %v", err)
	}

	expectedJSON := `{"downlink_message":{"f_port":1,"frm_payload":"dGVzdA==","priority":"NORMAL"}}`
	if string(payload) != expectedJSON {
		t.Errorf("Expected JSON:\n%s\nGot:\n%s", expectedJSON, string(payload))
	}
}
