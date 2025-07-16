package chunker

import (
	"bytes"
	"testing"
)

func TestChunkPayload(t *testing.T) {
	tests := []struct {
		name     string
		data     []byte
		maxSize  int
		expected [][]byte
		hasError bool
	}{
		{
			name:     "Empty data",
			data:     []byte{},
			maxSize:  10,
			expected: [][]byte{},
			hasError: false,
		},
		{
			name:     "Data smaller than maxSize",
			data:     []byte("hello"),
			maxSize:  10,
			expected: [][]byte{[]byte("hello")},
			hasError: false,
		},
		{
			name:     "Data exactly maxSize",
			data:     []byte("hello world"),
			maxSize:  11,
			expected: [][]byte{[]byte("hello world")},
			hasError: false,
		},
		{
			name:     "Data larger than maxSize",
			data:     []byte("hello world this is a test"),
			maxSize:  10,
			expected: [][]byte{
				[]byte("hello worl"),
				[]byte("d this is "),
				[]byte("a test"),
			},
			hasError: false,
		},
		{
			name:     "MaxSize 1",
			data:     []byte("abc"),
			maxSize:  1,
			expected: [][]byte{
				[]byte("a"),
				[]byte("b"),
				[]byte("c"),
			},
			hasError: false,
		},
		{
			name:     "Invalid maxSize",
			data:     []byte("hello"),
			maxSize:  0,
			expected: nil,
			hasError: true,
		},
		{
			name:     "Negative maxSize",
			data:     []byte("hello"),
			maxSize:  -1,
			expected: nil,
			hasError: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			chunks, err := ChunkPayload(tt.data, tt.maxSize)

			if tt.hasError {
				if err == nil {
					t.Errorf("Expected error but got none")
				}
				return
			}

			if err != nil {
				t.Errorf("Unexpected error: %v", err)
				return
			}

			if len(chunks) != len(tt.expected) {
				t.Errorf("Expected %d chunks, got %d", len(tt.expected), len(chunks))
				return
			}

			for i, chunk := range chunks {
				if !bytes.Equal(chunk, tt.expected[i]) {
					t.Errorf("Chunk %d mismatch: expected %q, got %q", i, tt.expected[i], chunk)
				}
			}
		})
	}
}

func TestChunkPayloadSizes(t *testing.T) {
	data := []byte("This is a test message that will be chunked into pieces of 51 bytes max")
	maxSize := 51

	chunks, err := ChunkPayload(data, maxSize)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Verify all chunks except the last one are exactly maxSize
	for i := 0; i < len(chunks)-1; i++ {
		if len(chunks[i]) != maxSize {
			t.Errorf("Chunk %d size: expected %d, got %d", i, maxSize, len(chunks[i]))
		}
	}

	// Verify the last chunk is <= maxSize
	if len(chunks) > 0 && len(chunks[len(chunks)-1]) > maxSize {
		t.Errorf("Last chunk size too large: expected <= %d, got %d", maxSize, len(chunks[len(chunks)-1]))
	}

	// Verify when reassembled, we get the original data
	var reassembled []byte
	for _, chunk := range chunks {
		reassembled = append(reassembled, chunk...)
	}

	if !bytes.Equal(reassembled, data) {
		t.Errorf("Reassembled data doesn't match original")
	}
}
