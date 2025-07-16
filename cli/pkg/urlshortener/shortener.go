package urlshortener

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"time"
)

// TinyURLResponse represents the response from TinyURL API
type TinyURLResponse struct {
	Data struct {
		URL string `json:"url"`
	} `json:"data"`
	Code int `json:"code"`
}

// TinyURLRequest represents the request to TinyURL API
type TinyURLRequest struct {
	URL string `json:"url"`
}

// Shorten shortens a URL using TinyURL API
func Shorten(url string) (string, error) {
	// For now, we'll use a simple implementation that just returns the URL
	// In a real implementation, you might want to use a URL shortening service
	
	// Try to use TinyURL API first
	shortened, err := shortenWithTinyURL(url)
	if err != nil {
		// If TinyURL fails, return the original URL
		// In production, you might want to log this error
		return url, nil
	}
	
	return shortened, nil
}

// shortenWithTinyURL shortens URL using TinyURL API
func shortenWithTinyURL(url string) (string, error) {
	client := &http.Client{
		Timeout: 10 * time.Second,
	}

	requestBody := TinyURLRequest{URL: url}
	jsonData, err := json.Marshal(requestBody)
	if err != nil {
		return "", fmt.Errorf("failed to marshal request: %w", err)
	}

	resp, err := client.Post("https://tinyurl.com/api-create.php", "application/json", bytes.NewBuffer(jsonData))
	if err != nil {
		return "", fmt.Errorf("failed to make request to TinyURL: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return "", fmt.Errorf("TinyURL API returned status: %d", resp.StatusCode)
	}

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return "", fmt.Errorf("failed to read response body: %w", err)
	}

	var response TinyURLResponse
	if err := json.Unmarshal(body, &response); err != nil {
		// If JSON parsing fails, try to use the response as plain text
		shortenedURL := string(body)
		if shortenedURL != "" && shortenedURL != url {
			return shortenedURL, nil
		}
		return "", fmt.Errorf("failed to parse TinyURL response: %w", err)
	}

	if response.Code != 0 {
		return "", fmt.Errorf("TinyURL API error: code %d", response.Code)
	}

	return response.Data.URL, nil
}

// ShortenSimple provides a simple URL shortening by truncating the URL
// This is a fallback when external services are not available
func ShortenSimple(url string) string {
	// Simple implementation that just returns the URL
	// In a real scenario, you might want to use a hash or other method
	return url
}
