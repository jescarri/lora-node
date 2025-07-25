package crypto

import (
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"os"
	"strings"

	"golang.org/x/crypto/nacl/sign"
)

// LoadPrivateKey loads a private key from the specified file path
// The key should be in hex format as generated by sodium-native-keygen
func LoadPrivateKey(path string) (*[64]byte, error) {
	keyData, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read private key file: %w", err)
	}

	// Clean up the key data (remove whitespace and newlines)
	keyHex := strings.TrimSpace(string(keyData))
	keyHex = strings.ReplaceAll(keyHex, "\n", "")
	keyHex = strings.ReplaceAll(keyHex, "\r", "")
	keyHex = strings.ReplaceAll(keyHex, " ", "")

	// Decode hex string to bytes
	keyBytes, err := hex.DecodeString(keyHex)
	if err != nil {
		return nil, fmt.Errorf("failed to decode hex private key: %w", err)
	}

	// Private key should be 64 bytes for Ed25519
	if len(keyBytes) != 64 {
		return nil, fmt.Errorf("invalid private key length: expected 64 bytes, got %d", len(keyBytes))
	}

	var privateKey [64]byte
	copy(privateKey[:], keyBytes)

	return &privateKey, nil
}

// Sign signs the data with the private key and returns the signature
func Sign(data []byte, privateKey *[64]byte) ([]byte, error) {
	signedMessage := sign.Sign(nil, data, privateKey)
	
	// The signed message contains the signature (64 bytes) + original message
	// We only want the signature part
	if len(signedMessage) < 64 {
		return nil, fmt.Errorf("signed message too short")
	}
	
	signature := signedMessage[:64]
	return signature, nil
}

// GenerateKeyPair generates a new Ed25519 key pair for testing
func GenerateKeyPair() (*[32]byte, *[64]byte, error) {
	publicKey, privateKey, err := sign.GenerateKey(rand.Reader)
	if err != nil {
		return nil, nil, fmt.Errorf("failed to generate key pair: %w", err)
	}

	return publicKey, privateKey, nil
}

// SavePrivateKey saves a private key to a file in hex format
func SavePrivateKey(privateKey *[64]byte, path string) error {
	keyHex := hex.EncodeToString(privateKey[:])
	return os.WriteFile(path, []byte(keyHex), 0600)
}

// SavePublicKey saves a public key to a file in hex format
func SavePublicKey(publicKey *[32]byte, path string) error {
	keyHex := hex.EncodeToString(publicKey[:])
	return os.WriteFile(path, []byte(keyHex), 0644)
}
