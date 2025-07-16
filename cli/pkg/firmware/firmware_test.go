package firmware

import (
	"net/http"
	"net/http/httptest"
	"testing"
)

func TestDownloadSuccess(t *testing.T) {
	// Create a test server
	testData := []byte("test firmware data")
	server := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusOK)
		w.Write(testData)
	}))
	defer server.Close()

	// Download the firmware
	data, err := Download(server.URL)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Verify the data
	if string(data) != string(testData) {
		t.Errorf("Expected %q, got %q", testData, data)
	}
}

func TestDownloadNotFound(t *testing.T) {
	// Create a test server that returns 404
	server := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusNotFound)
	}))
	defer server.Close()

	// Download the firmware
	_, err := Download(server.URL)
	if err == nil {
		t.Fatal("Expected error but got none")
	}
}

func TestDownloadServerError(t *testing.T) {
	// Create a test server that returns 500
	server := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusInternalServerError)
	}))
	defer server.Close()

	// Download the firmware
	_, err := Download(server.URL)
	if err == nil {
		t.Fatal("Expected error but got none")
	}
}

func TestDownloadInvalidURL(t *testing.T) {
	// Try to download from an invalid URL
	_, err := Download("invalid-url")
	if err == nil {
		t.Fatal("Expected error but got none")
	}
}
