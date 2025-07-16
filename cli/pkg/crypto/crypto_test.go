package crypto

import (
	"bytes"
	"encoding/hex"
	"os"
	"path/filepath"
	"testing"
	"golang.org/x/crypto/nacl/sign"
)

func TestGenerateKeyPair(t *testing.T) {
	publicKey, privateKey, err := GenerateKeyPair()
	if err != nil {
		t.Fatalf("Failed to generate key pair: %v", err)
	}

	if len(publicKey) != 32 {
		t.Errorf("Expected public key length 32, got %d", len(publicKey))
	}

	if len(privateKey) != 64 {
		t.Errorf("Expected private key length 64, got %d", len(privateKey))
	}
}

func TestSaveAndLoadPrivateKey(t *testing.T) {
	// Generate a key pair
	_, privateKey, err := GenerateKeyPair()
	if err != nil {
		t.Fatalf("Failed to generate key pair: %v", err)
	}

	// Create a temporary file
	tempDir := t.TempDir()
	keyPath := filepath.Join(tempDir, "private_key.hex")

	// Save the private key
	if err := SavePrivateKey(privateKey, keyPath); err != nil {
		t.Fatalf("Failed to save private key: %v", err)
	}

	// Load the private key
	loadedKey, err := LoadPrivateKey(keyPath)
	if err != nil {
		t.Fatalf("Failed to load private key: %v", err)
	}

	// Compare the keys
	if !bytes.Equal(privateKey[:], loadedKey[:]) {
		t.Errorf("Loaded key doesn't match original key")
	}
}

func TestLoadPrivateKeyWithWhitespace(t *testing.T) {
	// Generate a key pair
	_, privateKey, err := GenerateKeyPair()
	if err != nil {
		t.Fatalf("Failed to generate key pair: %v", err)
	}

	// Create a temporary file with whitespace
	tempDir := t.TempDir()
	keyPath := filepath.Join(tempDir, "private_key.hex")

	keyHex := hex.EncodeToString(privateKey[:])
	keyWithWhitespace := "  " + keyHex + "  \n\r  "
	if err := os.WriteFile(keyPath, []byte(keyWithWhitespace), 0600); err != nil {
		t.Fatalf("Failed to write key file: %v", err)
	}

	// Load the private key
	loadedKey, err := LoadPrivateKey(keyPath)
	if err != nil {
		t.Fatalf("Failed to load private key: %v", err)
	}

	// Compare the keys
	if !bytes.Equal(privateKey[:], loadedKey[:]) {
		t.Errorf("Loaded key doesn't match original key")
	}
}

func TestLoadPrivateKeyErrors(t *testing.T) {
	tempDir := t.TempDir()

	tests := []struct {
		name     string
		keyData  string
		expected string
	}{
		{
			name:     "Invalid hex",
			keyData:  "invalid_hex_data",
			expected: "failed to decode hex private key",
		},
		{
			name:     "Wrong length",
			keyData:  "deadbeef",
			expected: "invalid private key length",
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			keyPath := filepath.Join(tempDir, tt.name+".hex")
			if err := os.WriteFile(keyPath, []byte(tt.keyData), 0600); err != nil {
				t.Fatalf("Failed to write key file: %v", err)
			}

			_, err := LoadPrivateKey(keyPath)
			if err == nil {
				t.Errorf("Expected error but got none")
			}
		})
	}
}

func TestSignAndVerify(t *testing.T) {
	// Generate a key pair
	publicKey, privateKey, err := GenerateKeyPair()
	if err != nil {
		t.Fatalf("Failed to generate key pair: %v", err)
	}

	// Test data
	testData := []byte("test message for signing")

	// Sign the data
	signature, err := Sign(testData, privateKey)
	if err != nil {
		t.Fatalf("Failed to sign data: %v", err)
	}

	// Verify the signature length
	if len(signature) != 64 {
		t.Errorf("Expected signature length 64, got %d", len(signature))
	}

	// Verify the signature using Go's nacl/sign package
	var pubKey [32]byte
	copy(pubKey[:], publicKey[:])

	// Reconstruct the signed message for verification
	signedMessage := append(signature, testData...)
	
	// Verify the signature
	_, success := sign.Open(nil, signedMessage, &pubKey)
	if !success {
		t.Errorf("Signature verification failed")
	}
}

func TestSignEmpty(t *testing.T) {
	// Generate a key pair
	_, privateKey, err := GenerateKeyPair()
	if err != nil {
		t.Fatalf("Failed to generate key pair: %v", err)
	}

	// Sign empty data
	signature, err := Sign([]byte{}, privateKey)
	if err != nil {
		t.Fatalf("Failed to sign empty data: %v", err)
	}

	// Signature should still be 64 bytes
	if len(signature) != 64 {
		t.Errorf("Expected signature length 64, got %d", len(signature))
	}
}
