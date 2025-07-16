package chunker

import "fmt"

// ChunkPayload chunks the data into parts with a given maximum size
func ChunkPayload(data []byte, maxSize int) ([][]byte, error) {
	if maxSize <= 0 {
		return nil, fmt.Errorf("maxSize must be greater than 0")
	}

	var chunks [][]byte
	for len(data) > 0 {
		if len(data) > maxSize {
			chunks = append(chunks, data[:maxSize])
			data = data[maxSize:]
		} else {
			chunks = append(chunks, data)
			break
		}
	}

	return chunks, nil
}
