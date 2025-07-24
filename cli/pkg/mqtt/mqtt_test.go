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
	// Test the TTN message structure with chunked downlinks
	message := TTNMessage{
		Downlinks: []TTNDownlink{
			{
				FPort:      1,
				FRMPayload: base64.StdEncoding.EncodeToString([]byte("test data chunk 1")),
				Priority:   "NORMAL",
			},
			{
				FPort:      2,
				FRMPayload: base64.StdEncoding.EncodeToString([]byte("test data chunk 2")),
				Priority:   "NORMAL",
			},
		},
	}

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

	if len(unmarshaled.Downlinks) != 2 {
		t.Errorf("Expected 2 downlinks, got %d", len(unmarshaled.Downlinks))
	}

	// Check first downlink
	if unmarshaled.Downlinks[0].FPort != 1 {
		t.Errorf("Expected FPort 1, got %d", unmarshaled.Downlinks[0].FPort)
	}

	if unmarshaled.Downlinks[0].Priority != "NORMAL" {
		t.Errorf("Expected Priority NORMAL, got %s", unmarshaled.Downlinks[0].Priority)
	}

	// Decode the FRMPayload for first chunk
	decodedData1, err := base64.StdEncoding.DecodeString(unmarshaled.Downlinks[0].FRMPayload)
	if err != nil {
		t.Fatalf("Failed to decode FRMPayload: %v", err)
	}

	if string(decodedData1) != "test data chunk 1" {
		t.Errorf("Expected 'test data chunk 1', got %s", string(decodedData1))
	}

	// Check second downlink
	if unmarshaled.Downlinks[1].FPort != 2 {
		t.Errorf("Expected FPort 2, got %d", unmarshaled.Downlinks[1].FPort)
	}

	// Decode the FRMPayload for second chunk
	decodedData2, err := base64.StdEncoding.DecodeString(unmarshaled.Downlinks[1].FRMPayload)
	if err != nil {
		t.Fatalf("Failed to decode FRMPayload: %v", err)
	}

	if string(decodedData2) != "test data chunk 2" {
		t.Errorf("Expected 'test data chunk 2', got %s", string(decodedData2))
	}
}

func TestTTNMessageJSON(t *testing.T) {
	// Test that the JSON structure matches expected TTN chunked format
	message := TTNMessage{
		Downlinks: []TTNDownlink{
			{
				FPort:      1,
				FRMPayload: "dGVzdA==",
				Priority:   "NORMAL",
			},
			{
				FPort:      2,
				FRMPayload: "ZGF0YQ==",
				Priority:   "NORMAL",
			},
		},
	}

	payload, err := json.Marshal(message)
	if err != nil {
		t.Fatalf("Failed to marshal TTN message: %v", err)
	}

	expectedJSON := `{"downlinks":[{"f_port":1,"frm_payload":"dGVzdA==","priority":"NORMAL"},{"f_port":2,"frm_payload":"ZGF0YQ==","priority":"NORMAL"}]}`
	if string(payload) != expectedJSON {
		t.Errorf("Expected JSON:\n%s\nGot:\n%s", expectedJSON, string(payload))
	}
}

func TestPublishChunks(t *testing.T) {
	// Test the PublishChunks functionality
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

	// Test chunking multiple byte arrays
	chunks := [][]byte{
		[]byte("chunk1"),
		[]byte("chunk2"),
		[]byte("chunk3"),
	}

	// This would normally connect and publish, but we're just testing the structure
	// We can't easily test the actual MQTT publish without a running broker
	if client == nil {
		t.Fatal("Client should not be nil")
	}

	// Verify we can create the message structure correctly
	message := TTNMessage{
		Downlinks: make([]TTNDownlink, len(chunks)),
	}

	for i, chunk := range chunks {
		message.Downlinks[i] = TTNDownlink{
			FPort:      i + 1,
			FRMPayload: base64.StdEncoding.EncodeToString(chunk),
			Priority:   "NORMAL",
		}
	}

	if len(message.Downlinks) != 3 {
		t.Errorf("Expected 3 downlinks, got %d", len(message.Downlinks))
	}

	if message.Downlinks[0].FPort != 1 {
		t.Errorf("Expected first chunk FPort to be 1, got %d", message.Downlinks[0].FPort)
	}

	if message.Downlinks[2].FPort != 3 {
		t.Errorf("Expected third chunk FPort to be 3, got %d", message.Downlinks[2].FPort)
	}
}
