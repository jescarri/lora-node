# Build stage
FROM golang:1.23-alpine AS builder

# Install git for go modules
RUN apk add --no-cache git

# Set working directory
WORKDIR /app

# Copy go mod files
COPY go.mod go.sum ./

# Download dependencies
RUN go mod download

# Copy source code
COPY . .

# Build the application
RUN CGO_ENABLED=0 GOOS=linux go build -a -installsuffix cgo -o lora-sensor-cli .

# Test stage
FROM builder AS tester

# Run tests
RUN go test ./... -v

# Final stage
FROM alpine:latest

# Install ca-certificates for HTTPS requests
RUN apk --no-cache add ca-certificates

# Create non-root user
RUN addgroup -g 1001 -S appgroup && \
    adduser -u 1001 -S appuser -G appgroup

# Set working directory
WORKDIR /root/

# Copy the binary from builder stage
COPY --from=builder /app/lora-sensor-cli .

# Change ownership to non-root user
RUN chown appuser:appgroup lora-sensor-cli

# Switch to non-root user
USER appuser

# Expose port (if needed)
EXPOSE 8080

# Command to run the application
ENTRYPOINT ["./lora-sensor-cli"]
